#include <arpa/inet.h>
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
#include "hmac.h"
#include "misc.h"

using namespace std;
using namespace boost::multiprecision;

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
  
  if (prime != (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f") {
    if (fermat_primality_test(prime, 128) == false) {
      throw invalid_argument("prime [" + prime.str() + "] is not a prime number");
    }
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

string FieldElement::to_string(bool inHex) {
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
    throw invalid_argument("Point (" + this->x_.to_string() + ", " + this->y_.to_string() +") not on the curve");
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

string FieldElementPoint::to_string(bool inHex) {
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



const int512_t S256Element::s256_prime_ = (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f";

S256Element::S256Element(int512_t num): FieldElement(num, S256Element::s256_prime_) {}

string S256Element::to_string() {
  return FieldElement::to_string(true);
}

int512_t S256Element::s256_prime() {
  return this->s256_prime_;
}

S256Element S256Element::power(const int512_t exponent) {
  return S256Element(powm(this->num_, exponent, this->prime_));
  
}

S256Element S256Element::sqrt() {
  return this->power((this->prime() + 1) / 4);  
}




const S256Element S256Point::a_ = S256Element(0);
const S256Element S256Point::b_ = S256Element(7);
const int512_t S256Point::order_ = (int512_t)"0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141";

S256Point::S256Point(S256Element x, S256Element y): FieldElementPoint(x, y, S256Point::a_, S256Point::b_) {}
S256Point::S256Point(): FieldElementPoint(S256Point::a_, S256Point::b_) {}

int512_t S256Point::s256_prime() {
  assert (this->x_.prime() == this->y_.prime());
  return this->x_.prime();
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

bool S256Point::verify(int512_t msg_hash, Signature sig) {

  int512_t sigInv = boost::integer::mod_inverse(sig.s(), this->order());
  int512_t u = msg_hash * sigInv % this->order();
  int512_t v = sig.r() * sigInv % this->order();

  S256Point total = G * u + *this * v;
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

string S256Point::to_string() {
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

unsigned char* S256Point::get_sec_format(const bool compressed = true) {
  // we can't use sizeof(int256_t) instead of KEY_SIZE = 32--sizeof(int256_t) is 48, not 32
  const int KEY_SIZE = 32;
  unsigned char* sec_bytes;
  assert (this->x().num() <= (int512_t)"0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  assert (this->y().num() <= (int512_t)"0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  
  unsigned char x_[KEY_SIZE];
  unsigned char y_[KEY_SIZE];

  get_bytes_from_int256((int256_t)this->x().num(), true, x_);
  get_bytes_from_int256((int256_t)this->y().num(), true, y_);

  if (compressed == false) {
    sec_bytes = (unsigned char*)calloc(1 + KEY_SIZE * 2, 1);
    sec_bytes[0] = 0x04;

    memcpy(sec_bytes + 1, x_, KEY_SIZE);
    memcpy(sec_bytes + 1 + KEY_SIZE, y_, KEY_SIZE);
  } else {
    sec_bytes = (unsigned char*)calloc(1 + KEY_SIZE, 1);
    sec_bytes[0] = this->y().num() % 2 == 0 ? 0x02 : 0x03;    
    memcpy(sec_bytes + 1, x_, KEY_SIZE);
  }
  return sec_bytes;
}

char* S256Point::get_address(bool compressed, bool testnet) {
  const size_t sec_len = compressed ? (1 + 32) : (1 + 32 * 2);
  unsigned char* sec_bytes = this->get_sec_format(compressed);
  unsigned char hash[RIPEMD160_HASH_SIZE+1];
  hash160(sec_bytes, sec_len, hash+1);
  free(sec_bytes);
  hash[0] = testnet ? 0x6f : 0x00;
  return encode_base58_checksum(hash, RIPEMD160_HASH_SIZE+1);
}


Signature::Signature(int512_t r, int512_t s) {
  this->r_ = r;
  this->s_ = s;
}

string Signature::to_string() {
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

unsigned char* Signature::get_der_format(size_t* output_len) {
  /*
   * DER format explained:
   * [30][45][02][20][37206a0610995c58074999cb9767b87af4c4978db68c06e8e6e81d282047a7c6][02][21][008ca63759c1157ebeaec0d03cecca119fc9a75bf8e6d0fa65c841c8e2738cdaec]
   * [30]       - Marker
   * [45]       - Length of the entire signature (i.e., length of the hex string - 4)
   * [02]       - Marker for r value
   * [20]       - r value length
   * [37... c6] - r value
   * [02]       - Marker for s value
   * [21]       - s value length
   * [00....ec] - s value
   */

  const size_t INT256_SIZE = 32;

  size_t r_pos = 1;
  size_t s_pos = 1;
  unsigned char* r_bytes = (unsigned char*)calloc(INT256_SIZE + 1, sizeof(unsigned char));
  unsigned char* s_bytes = (unsigned char*)calloc(INT256_SIZE + 1, sizeof(unsigned char));
  // the extra 1 byte is reserved for the possible prepending of 0x00

  get_bytes_from_int256((int256_t)this->r(), true, r_bytes + 1);
  get_bytes_from_int256((int256_t)this->s(), true, s_bytes + 1);
  // r_bytes[0]/s_bytes[0] is for the possible 0x00 prepending
  while (r_bytes[r_pos] == 0x00) { r_pos ++; }
  while (s_bytes[s_pos] == 0x00) { s_pos ++; }

  if (r_bytes[r_pos] >> 7 == 1) { r_bytes[--r_pos] = 0x00; }
  if (s_bytes[s_pos] >> 7 == 1) { s_bytes[--s_pos] = 0x00; }
  size_t r_bytes_stripped_len = INT256_SIZE + 1 - r_pos;
  size_t s_bytes_stripped_len = INT256_SIZE + 1 - s_pos;
  
  unsigned char* r_bytes_stripped = (unsigned char*)calloc(r_bytes_stripped_len, sizeof(unsigned char));
  unsigned char* s_bytes_stripped = (unsigned char*)calloc(s_bytes_stripped_len, sizeof(unsigned char));
  memcpy(r_bytes_stripped, r_bytes + r_pos, r_bytes_stripped_len);
  memcpy(s_bytes_stripped, s_bytes + s_pos, s_bytes_stripped_len);

  size_t results_len = 2 + 2 + r_bytes_stripped_len + 2 + s_bytes_stripped_len;
  unsigned char* results = (unsigned char*)calloc(results_len, sizeof(unsigned char));

  results[0] = 0x30;
  results[1] = results_len - 2; // results[0] and results[1] are not considered a part of the "results"..

  results[2] = 0x02;
  results[3] = r_bytes_stripped_len;
  // Here we don't - 2 as results[1], because r_bytes_stripped_len is defined as the length of the r_bytes only
  memcpy(results + 4, r_bytes_stripped, r_bytes_stripped_len);
  
  results[4 + r_bytes_stripped_len] = 0x02;
  results[5 + r_bytes_stripped_len] = s_bytes_stripped_len;
  memcpy(results + 2 + 2 + r_bytes_stripped_len + 2, s_bytes_stripped, s_bytes_stripped_len);
  
  free(r_bytes);
  free(r_bytes_stripped);
  free(s_bytes);
  free(s_bytes_stripped);
  *output_len = results_len;

  return results;
}



S256Point G = S256Point(
    S256Element((int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"),
    S256Element((int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8")
);

ECDSAKey::ECDSAKey(const unsigned char* private_key_bytes, const size_t private_key_length) {
  assert (private_key_length <= SHA256_HASH_SIZE);
  memcpy(this->privkey_bytes_ + (SHA256_HASH_SIZE - private_key_length), private_key_bytes, private_key_length);
  // Note the below lines are the same as ECDSAKey::ECDSAKey(const int512_t private_key);
  // But in C++ we cant call another constructor within an constructor easily.
  this->privkey_int_ = get_int512_from_bytes(this->privkey_bytes_, SHA256_HASH_SIZE);
  this->public_key_ = G * privkey_int_;  
}

ECDSAKey::ECDSAKey(const int512_t private_key) {
  this->privkey_int_ = private_key;
  get_bytes_from_int256((int256_t)private_key, true, this->privkey_bytes_);
  this->public_key_ = G * privkey_int_;
}

ECDSAKey::~ECDSAKey() {
  /*if (this->privkey_bytes_ != nullptr) {
    delete this->privkey_bytes_;
  }*/
}

string ECDSAKey::to_string() {
  stringstream ss;
  ss << hex << this->privkey_int_ << endl;
  return ss.str();
}

Signature ECDSAKey::sign(unsigned char* msgHashBytes, size_t msgHashLen) {
  int512_t k = this->get_deterministic_k(msgHashBytes, msgHashLen);
  cout << "k is " << k << endl;
  int512_t r = (G * k).x().num();
  int512_t kInv = boost::integer::mod_inverse(k, G.order());
  int512_t sig = (int512_t)((int1024_t)(get_int512_from_bytes(msgHashBytes, msgHashLen) + this->privkey_int_ * r) * kInv % G.order());
  // (msg_hash + this->privkey_int_ * r) * kInv may exceed the size of int512_t!
  if (sig > G.order() / 2) {
    sig = G.order() - sig;
  }
  return Signature(r, sig);
}

int512_t ECDSAKey::get_deterministic_k(unsigned char* msgHashBytes, size_t msgHashLen) {
  assert (msgHashLen == SHA256_HASH_SIZE);
  unsigned char kBytes[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  unsigned char vBytes[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
  };
  assert (sizeof(vBytes) == SHA256_HASH_SIZE && sizeof(kBytes) == SHA256_HASH_SIZE);
  int512_t msgHashInt = get_int512_from_bytes(msgHashBytes, SHA256_HASH_SIZE);
  if (msgHashInt > G.order()) {
    msgHashInt -= G.order();
  }

  unsigned short int dataLen = (SHA256_HASH_SIZE + 1 + SHA256_HASH_SIZE + SHA256_HASH_SIZE) * sizeof(unsigned char);
  unsigned char* data = (unsigned char*)malloc(dataLen * sizeof(unsigned char));
  memcpy(data, vBytes, SHA256_HASH_SIZE);
  data[SHA256_HASH_SIZE] = 0x00;
  memcpy(data + SHA256_HASH_SIZE + 1, this->privkey_bytes_, SHA256_HASH_SIZE);
  memcpy(data + SHA256_HASH_SIZE + 1 + SHA256_HASH_SIZE, msgHashBytes, SHA256_HASH_SIZE);
  unsigned char out[SHA256_HASH_SIZE];
  hmac_sha256(kBytes, SHA256_HASH_SIZE, data, dataLen, kBytes);
  hmac_sha256(kBytes, SHA256_HASH_SIZE, vBytes, SHA256_HASH_SIZE, vBytes);

  memcpy(data, vBytes, SHA256_HASH_SIZE);
  data[SHA256_HASH_SIZE] = 0x01;
  memcpy(data + SHA256_HASH_SIZE + 1, this->privkey_bytes_, SHA256_HASH_SIZE);
  memcpy(data + SHA256_HASH_SIZE + 1 + SHA256_HASH_SIZE, msgHashBytes, SHA256_HASH_SIZE);
  
  hmac_sha256(kBytes, SHA256_HASH_SIZE, data, dataLen, kBytes);  
  hmac_sha256(kBytes, SHA256_HASH_SIZE, vBytes, SHA256_HASH_SIZE, vBytes);
  while (true) {
    hmac_sha256(kBytes, SHA256_HASH_SIZE, vBytes, SHA256_HASH_SIZE, vBytes);
    int512_t candidate = get_int512_from_bytes(vBytes, SHA256_HASH_SIZE);
    if (candidate >= 1 && candidate < G.order()) {
      delete[] data;
      return candidate;
    }
    cout << "\n\n\n=====WARNING=====\nThis is a route that seldom tested, result may well be wrong!!\n\n\n" << endl;
    memcpy(data, vBytes, SHA256_HASH_SIZE);
    data[SHA256_HASH_SIZE] = 0x00;
    hmac_sha256(kBytes, SHA256_HASH_SIZE, data, SHA256_HASH_SIZE + 1, kBytes);
    // Here we only pass a part of data to the function!
    hmac_sha256(kBytes, SHA256_HASH_SIZE, vBytes, SHA256_HASH_SIZE, vBytes);
  }
}

S256Point ECDSAKey::public_key() {
  return this->public_key_;
}

char* ECDSAKey::get_wif_private_key(bool compressed, bool testnet) {
  const size_t input_len = (compressed ? 34 : 33);
  unsigned char input[input_len] = {0};
  input[0] = (testnet ? 0xef : 0x80);
  if (compressed) { input[33] = 0x01; }  
  memcpy(input + 1, this->privkey_bytes_, 32);
  return encode_base58_checksum(input, input_len);
}