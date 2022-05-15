#include <assert.h>
#include <boost/integer/mod_inverse.hpp>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "ecc.h"
#include "utils.h"
#include "hmac-sha256.h"
#include "sha256.h"

using namespace std;
using namespace boost::multiprecision;


bool FieldElement::isPrimeNumber(int512_t input) {

    if (input < 2) { return false; }
    if (input == 2) { return true; }
    int512_t secp256k1Prime = (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f";
    // secp256k1Prime = 2^256 - 2^32 - 977
    if (input == secp256k1Prime) { return true; }
    if (input % 2 == 0) { return false; }
    if (input > 2147483647) {
      cout << "Input " << input << " is VERY LARGE!!!" << endl;
    }
    for (int512_t i = 3; (i*i)<=input; i+=2) {
        if(input % i == 0 ) { return false; }
    }
    return true;
}

FieldElement::FieldElement(int512_t num, int512_t prime) {
  if (num >= prime) {
    throw invalid_argument("invalid num [" + num.str() + "] is negative or greater than prime [" + prime.str() + "]");
  }
  if (num > (int512_t)"0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff") { // avoid risk of overflow
    throw invalid_argument("num [" + num.str() + "] is longer than 256 bits, which is not supported");
  }
  if (prime > (int512_t)"0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff") {  // avoid risk of overflow
    throw invalid_argument("prime [" + num.str() + "] is longer than 256 bits, which is not supported");
  }
  if (!isPrimeNumber(prime)) {
    throw invalid_argument("prime [" + prime.str() + "] is not a prime number");
  }
  this->num_ = num;
  this->prime_ = prime;
}

FieldElement::FieldElement() {
  // This is needed if we want to declare an object without defining it
  // It will be defined with this default constructor automatically.
  
  // FieldElement(0, 1); seems in C++ we can't call another constructor in a constructor...  
}

int512_t FieldElement::num() {
  return this->num_;
}

int512_t FieldElement::prime() {
  return this->prime_;
}

bool FieldElement::operator==(const FieldElement& other) const
{
  // ICYW: This is overloading, not overriding lol. One point to note is that while operator==() is supposed to be a
  // member method of this, it can  access private members of other as well.
  return this->prime_ == other.prime_ && this->num_ == other.num_;
}

bool FieldElement::operator!=(const FieldElement& other) const
{
  // ICYW: This is overloading, not overriding lol
  return !(*this == other);
}

FieldElement FieldElement::operator+(const FieldElement& other)
{
  // ICYW: This is overloading, not overriding lol
  if (this->prime_ != other.prime_) {
    throw std::invalid_argument("prime numbers are different");
  }
  return FieldElement((this->num_ + other.num_) % this->prime_, this->prime_);
}

FieldElement FieldElement::operator-(const FieldElement& other)
{
  // ICYW: This is overloading, not overriding lol
  if (this->prime_ != other.prime_) {
    throw std::invalid_argument("prime numbers are different");
  }
  int512_t result = (this->num_ - other.num_) % this->prime_;
  if (result < 0) { result += this->prime_; }
  return FieldElement((int512_t)result, this->prime_);
}

FieldElement FieldElement::operator*(const FieldElement& other)
{
  if (this->prime_ != other.prime_) {
    throw std::invalid_argument("prime numbers are different");
  }
  return FieldElement((this->num_ * other.num_) % this->prime_, this->prime_);
}

FieldElement FieldElement::operator*(const int512_t other)
{
  // This implementation is inspired by FieldElementPoint FieldElementPoint::operator*(const int other)
  int512_t coef = other;
  FieldElement result = FieldElement(0, this->prime());
  FieldElement curr = FieldElement(*this);
  while (coef > 0) {
    if (coef & 1) {
      result = result + curr;
    }
    curr = curr + curr;
    coef >>= 1;
  }
  return result;
}

FieldElement FieldElement::operator/(const FieldElement& other)
{
  if (this->prime_ != other.prime_) {
    throw std::invalid_argument("prime numbers are different");
  }
  int512_t tmp = FieldElement(other.num_, other.prime_).power(other.prime_-2).num_;
  return FieldElement(
    (this->num_ * tmp) % this->prime_,
    this->prime_
  );
}

FieldElement FieldElement::power(int512_t exponent)
{  
  return FieldElement(powm(this->num_, exponent, this->prime_), this->prime_);
}

string FieldElement::toString(bool inHex) {
  stringstream ss;
  if (inHex) { ss << std::hex; }
  ss << this->num_ << " (" << this->prime_ << ")";
  return ss.str();
}



FieldElementPoint::FieldElementPoint(FieldElement x, FieldElement y, FieldElement a, FieldElement b) {
  //if (x.prime() == y.prime() == a.prime() == b.prime()) { /*good*/ }
  //else { throw invalid_argument("prime numbers are different"); }

  this->x_ = FieldElement(x.num(), x.prime());
  this->y_ = FieldElement(y.num(), y.prime());
  this->a_ = FieldElement(a.num(), a.prime());
  this->b_ = FieldElement(b.num(), b.prime());

  if (this->y_.power(2) != this->x_.power(3) + (this->a_ * this->x_) + this->b_) {    
    throw invalid_argument("Point (" + this->x_.toString() + ", " + this->y_.toString() +") not on the curve");
  }
}

// Omitting x and y and passing only a and b in y^2 = x^3 + ax + b means this FieldElementPoint is point at infinity
FieldElementPoint::FieldElementPoint(FieldElement a, FieldElement b) {
  if (a.prime() == b.prime()) { /*good*/ }
  else { throw invalid_argument("prime numbers are different"); }

  this->infinity_ = true;
  this->a_ = FieldElement(a.num(), a.prime());
  this->b_ = FieldElement(b.num(), b.prime());  
}

FieldElementPoint::~FieldElementPoint() {}

bool FieldElementPoint::operator==(const FieldElementPoint& other) const
{
  // ICYW: This is overloading, not overriding. An interesting point to note is that a,b,x,y are private members
  // of other, but still we can access them.
  bool xyEqual = false;
  if (this->infinity_ == true && other.infinity_ == true) { xyEqual = true; }
  else if (this->infinity_ != other.infinity_) { xyEqual = false; }
  else if (this->infinity_ == false && other.infinity_ == false) { xyEqual = (this->x_ == other.x_ && this->y_ == other.y_); }
  else { throw invalid_argument("This should be impossible"); }
  return this->a_ == other.a_ && this->b_ == other.b_ && xyEqual;
}

FieldElementPoint FieldElementPoint::operator+(const FieldElementPoint& other)
{  
  // ICYW: This is overloading, not overriding lol. An interesting point to note is that a,b,x,y are private members
  // of other, but still we can access them.
  if (this->a_ != other.a_ || this->b_ != other.b_) {
    throw std::invalid_argument("Two FieldElementPoints are not on the same curve");
  }

  // Point at infinity is defined as I where point A + I = A. Visualization: ./00_assets/fig_02-15.png
  if (this->infinity_) { return other; }
  if (other.infinity_) { return *this; }
  // meaning that A + (-A) = I. Visualization: ./00_assets/fig_02-15.png
  if (this->x_ == other.x_ &&  this->y_ != other.y_) {    
    return FieldElementPoint(this->a_, this->b_);
  }

  if (*this == other && this->y_ == FieldElement(0, this->y_.prime())) {
    // It means p1 == p2 and tangent is a vertical line. Visualization: ./00_assets/fig_02-19.png
    return FieldElementPoint(this->a_, this->b_);
  }  
  
  FieldElement slope;
  // What is the value of slope at the moment?
  //It is NOT left undefined--it will be defined by the default constructor!
  if (this->x_ == other.x_ && this->y_ == other.y_) {
    // p1 == p2, need some calculus to derive formula: (slope = 3x^2 + a) / 2y
    // Essentially this is the tangent line at P1. Visualization: ./00_assets/fig_02-18.png
    slope = (this->x_.power(2) * 3 + this->a_) / (this->y_ * 2);
  } else {
    // general case. Visualization: ./00_assets/fig_02-11.png
    slope = (this->y_ - other.y_) / (this->x_ - other.x_);
  }
  FieldElement x3 = FieldElement(slope * slope - this->x_ - other.x_);
  FieldElement y3 = FieldElement(slope * (this->x_ - x3) - this->y_);
  return FieldElementPoint(x3, y3, this->a_, this->b_);
  
}

FieldElementPoint FieldElementPoint::operator+=(const FieldElementPoint& other)
{
  *this = *this + other;
  return *this;
}

FieldElementPoint FieldElementPoint::operator*(const int512_t other)
{
  // The current implementation is called "binary expansion". It reduces the time complexity from O(n) to Olog(n).
  // Essentially, we scan coef bit by bit and add the number to result if a bit is 1.
  int512_t coef = other;
  FieldElementPoint result = FieldElementPoint(this->a_, this->b_);
  FieldElementPoint curr = FieldElementPoint(*this);
  /*
    for (int i = 0; i < other; i++) {
      result = FieldElementPoint(*this + result);
    }
  */
  while (coef > 0) {
    if (coef & 1) { // &: bitwise AND, check if the rightmost bit of coef == 1
      result = result + curr;
    }
    curr = curr + curr;
    // Can we use curr = curr * 2? NO! Since this is exactly where multiplication is defined! It causes a stack overflow
    // if we use * in FieldElementPoint::operator*()
    coef >>= 1; 
    
    // >>: right shift the bits of the first operand by the second operand number of places.
    // 14:       0000 1110
    // 14 >> 1:  0000 0111 = 7
    // 14 >> 2:  0000 0011 = 3
  }
  return result;
}

string FieldElementPoint::toString(bool inHex) {
  stringstream ss;
  if (inHex) { ss << std::hex; }
  if (this->infinity_) {
    ss << "FieldElementPoint(Infinity)_" << this->a_.num() <<  "_" << this->b_.num() << " FieldElement(" << this->a_.prime() << ")";
  } else {
    ss << "FieldElementPoint(" << this->x_.num() << ", " << this->y_.num() << ")_"
       << this->a_.num() <<  "_" << this->b_.num() << " FieldElement(" << this->a_.prime() << ")";    
  }
  return ss.str();
}

bool FieldElementPoint::infinity() {
  return this->infinity_;
}

FieldElement FieldElementPoint::x() {
  return this->x_;
}

FieldElement FieldElementPoint::y() {
  return this->y_;
}



const int512_t S256Element::s256Prime_ = (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f";

S256Element::S256Element(int512_t num): FieldElement(num, S256Element::s256Prime_) {}

string S256Element::toString() {
  return FieldElement::toString(true);
}

int512_t S256Element::s256Prime() {
  return this->s256Prime_;
}

const S256Element S256Point::a_ = S256Element(0);
const S256Element S256Point::b_ = S256Element(7);
const int512_t S256Point::order_ = (int512_t)"0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141";

S256Point::S256Point(S256Element x, S256Element y): FieldElementPoint(x, y, S256Point::a_, S256Point::b_) {}
S256Point::S256Point(): FieldElementPoint(S256Point::a_, S256Point::b_) {}

int512_t S256Point::s256Prime() {
  assert (this->x_.prime() == this->y_.prime());
  return this->x_.prime();
}

bool S256Point::verify(int512_t msgHash, Signature sig) {

  int512_t gx = (int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798";
  int512_t gy = (int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8";  
  // gx and gy are the coordinates of point g, which is the generator point, i.e., we add g to itself over and over again.
  S256Point g = S256Point(S256Element(gx), S256Element(gy));

  int512_t sigInv = boost::integer::mod_inverse(sig.s(), this->order());
  int512_t u = msgHash * sigInv % this->order();
  int512_t v = sig.r() * sigInv % this->order();

  S256Point total = g * u + *this * v;
  return total.x().num() == sig.r();
}

S256Point S256Point::operator*(const int512_t other) {
  FieldElementPoint temp = FieldElementPoint::operator*(other);
  if (temp.infinity()) {
    return S256Point();
  }
  else {
    return S256Point(S256Element(temp.x().num()), S256Element(temp.y().num()));
  }
} 

S256Point S256Point::operator+(const S256Point other) {
  FieldElementPoint temp = (FieldElementPoint)(*this) + (FieldElementPoint)(other);
  if (temp.infinity()) {
    return S256Point();
  }
  else {
    return S256Point(S256Element(temp.x().num()), S256Element(temp.y().num()));
  }
}

string S256Point::toString() {
  stringstream ss;
  ss << std::hex;
  if (this->infinity_) {
    ss << "S256Point(Infinity)_" << this->a().num() <<  "_" << this->b().num();
  } else {
    ss << "S256Point("
       << setw(64) << setfill('0') << this->x_.num() << ", " 
       << setw(64) << setfill('0') << this->y_.num() << ")_"
       << this->a().num() <<  "_" << this->b().num();
  }
  return ss.str();
}

int512_t S256Point::order() {
  return S256Point::order_;
}

S256Element S256Point::a() {
  return S256Point::a_;
}

S256Element S256Point::b() {
  return S256Point::b_;
}

Signature::Signature(int512_t r, int512_t s) {
  this->r_ = r;
  this->s_ = s;
}

string Signature::toString() {
  stringstream ss;
  ss << hex << "Signature(" << this->r_ << ", " << this->s_ << ")";
  return ss.str();
}

int512_t Signature::r() {
  return this->r_;
}

int512_t Signature::s() {
  return this->s_;
}

S256Point G = S256Point(
    S256Element((int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"),
    S256Element((int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8")
);

ECDSAPrivateKey::ECDSAPrivateKey(unsigned char* secretBytes, unsigned short int secretLen) {
  assert (secretLen <= SIZE_OF_SHA_256_HASH);
  this->secretBytes_ = (unsigned char*)calloc(SIZE_OF_SHA_256_HASH, sizeof(unsigned char));
  memcpy(this->secretBytes_ + (SIZE_OF_SHA_256_HASH - secretLen), secretBytes, secretLen);
  this->secret_ = getInt512FromBytes(this->secretBytes_,  SIZE_OF_SHA_256_HASH);
  this->p_ = G * secret_;
}

ECDSAPrivateKey::~ECDSAPrivateKey() {
  if (secretBytes_ != nullptr) {
    delete[] this->secretBytes_;
  }
}

string ECDSAPrivateKey::toString() {
  stringstream ss;
  ss << hex << this->secret_ << endl;
  return ss.str();
}

Signature ECDSAPrivateKey::sign(unsigned char* msgHashBytes, unsigned short int msgHashLen) {
  int512_t k = this->getDeterministicK(msgHashBytes, msgHashLen);
  cout << "k is " << k << endl;
  int512_t r = (G * k).x().num();
  int512_t kInv = boost::integer::mod_inverse(k, G.order());
  int512_t sig = (int512_t)((int1024_t)(getInt512FromBytes(msgHashBytes, msgHashLen) + this->secret_ * r) * kInv % G.order());
  // (msgHash + this->secret_ * r) * kInv may exceed the size of int512_t!
  if (sig > G.order() / 2) {
    sig = G.order() - sig;
  }
  return Signature(r, sig);
}

int512_t ECDSAPrivateKey::getDeterministicK(unsigned char* msgHashBytes, unsigned short int msgHashLen) {
  // This big ugly thing is an implmentation of RFC 6979...
  assert (msgHashLen == SIZE_OF_SHA_256_HASH);
  assert (this->secretLen_ == SIZE_OF_SHA_256_HASH);
  unsigned char kBytes[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  unsigned char vBytes[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
  };
  assert (sizeof(vBytes) == SIZE_OF_SHA_256_HASH && sizeof(kBytes) == SIZE_OF_SHA_256_HASH);
  int512_t msgHashInt = getInt512FromBytes(msgHashBytes, SIZE_OF_SHA_256_HASH);
  if (msgHashInt > G.order()) {
    msgHashInt -= G.order();
  }

  unsigned short int dataLen = (SIZE_OF_SHA_256_HASH + 1 + this->secretLen_ + SIZE_OF_SHA_256_HASH) * sizeof(unsigned char);
  unsigned char* data = (unsigned char*)malloc(dataLen * sizeof(unsigned char));
  memcpy(data, vBytes, SIZE_OF_SHA_256_HASH);
  data[SIZE_OF_SHA_256_HASH] = 0x00;
  memcpy(data + SIZE_OF_SHA_256_HASH + 1, this->secretBytes_, this->secretLen_);
  memcpy(data + SIZE_OF_SHA_256_HASH + 1 + this->secretLen_, msgHashBytes, SIZE_OF_SHA_256_HASH);
  unsigned char out[SIZE_OF_SHA_256_HASH];
  hmac_sha256(kBytes, SIZE_OF_SHA_256_HASH, data, dataLen, kBytes);
  hmac_sha256(kBytes, SIZE_OF_SHA_256_HASH, vBytes, SIZE_OF_SHA_256_HASH, vBytes);

  memcpy(data, vBytes, SIZE_OF_SHA_256_HASH);
  data[SIZE_OF_SHA_256_HASH] = 0x01;
  memcpy(data + SIZE_OF_SHA_256_HASH + 1, this->secretBytes_, this->secretLen_);
  memcpy(data + SIZE_OF_SHA_256_HASH + 1 + this->secretLen_, msgHashBytes, SIZE_OF_SHA_256_HASH);
  
  hmac_sha256(kBytes, SIZE_OF_SHA_256_HASH, data, dataLen, kBytes);  
  hmac_sha256(kBytes, SIZE_OF_SHA_256_HASH, vBytes, SIZE_OF_SHA_256_HASH, vBytes);
  while (true) {
    hmac_sha256(kBytes, SIZE_OF_SHA_256_HASH, vBytes, SIZE_OF_SHA_256_HASH, vBytes);
    int512_t candidate = getInt512FromBytes(vBytes, SIZE_OF_SHA_256_HASH);
    if (candidate >= 1 && candidate < G.order()) {
      delete[] data;
      return candidate;
    }
    cout << "WARNING: This is a route that seldom tested, result may well be wrong!!" << endl;
    memcpy(data, vBytes, SIZE_OF_SHA_256_HASH);
    data[SIZE_OF_SHA_256_HASH] = 0x00;
    hmac_sha256(kBytes, SIZE_OF_SHA_256_HASH, data, SIZE_OF_SHA_256_HASH + 1, kBytes);
    // Here we only pass a part of data to the function!
    hmac_sha256(kBytes, SIZE_OF_SHA_256_HASH, vBytes, SIZE_OF_SHA_256_HASH, vBytes);
  }
}