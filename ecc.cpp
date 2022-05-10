#include <math.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <float.h>
#include <limits.h>
#include <sstream>
#include "ecc.h"

using namespace std;
using namespace boost::multiprecision;

bool FieldElement::isPrimeNumber(int512_t input) {

    if (input < 2) { return false; }
    if (input == 2) { return true; }
    int512_t secp256k1Prime{"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"};
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
    throw invalid_argument("invalid num [" + num.str() + "]: negative or greater than prime");
  }
  if (!isPrimeNumber(prime)) {
    throw invalid_argument("invalid prime [" + prime.str() + "]: not a prime number");
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
  // seems here we need to point the internal pointers to new objects--if we point to the same objects 
  // as x, y, a, b, if these objects get deleted by some other function, the this->x, this->y, this->a, this->b,
  // will point to nothing. 
  this->x = FieldElement(x.num(), x.prime());
  this->y = FieldElement(y.num(), y.prime());
  this->a = FieldElement(a.num(), a.prime());
  this->b = FieldElement(b.num(), b.prime());
  
  if (this->y.power(2) != this->x.power(3) + (this->a * this->x) + this->b) {    
    throw invalid_argument("Point (" + this->x.toString() + ", " + this->y.toString() +") not on the curve");
  }
}


// Omitting x and y and passing only a and b in y^2 = x^3 + ax + b means this FieldElementPoint is point at infinity
FieldElementPoint::FieldElementPoint(FieldElement a, FieldElement b) {
  this->infinity_ = true;
  this->a = FieldElement(a.num(), a.prime());
  this->b = FieldElement(b.num(), b.prime());  
}

FieldElementPoint::~FieldElementPoint() {}

bool FieldElementPoint::operator==(const FieldElementPoint& other) const
{
  // ICYW: This is overloading, not overriding. An interesting point to note is that a,b,x,y are private members
  // of other, but still we can access them.
  bool xyEqual = false;
  if (this->infinity_ == true && other.infinity_ == true) { xyEqual = true; }
  else if (this->infinity_ != other.infinity_) { xyEqual = false; }
  else if (this->infinity_ == false && other.infinity_ == false) { xyEqual = (this->x == other.x && this->y == other.y); }
  else { throw invalid_argument("This should be impossible"); }
  return this->a == other.a && this->b == other.b && xyEqual;
}

FieldElementPoint FieldElementPoint::operator+(const FieldElementPoint& other)
{  
  // ICYW: This is overloading, not overriding lol. An interesting point to note is that a,b,x,y are private members
  // of other, but still we can access them.
  if (this->a != other.a || this->b != other.b) {
    throw std::invalid_argument("Two FieldElementPoints are not on the same curve");
  }

  // Point at infinity is defined as I where point A + I = A. Visualization: ./00_assets/fig_02-15.png
  if (this->infinity_) { return other; }
  if (other.infinity_) { return *this; }
  // meaning that A + (-A) = I. Visualization: ./00_assets/fig_02-15.png
  if (this->x == other.x &&  this->y != other.y) {    
    return FieldElementPoint(this->a, this->b);
  }

  if (*this == other && this->y == FieldElement(0, this->y.prime())) {
    // It means p1 == p2 and tangent is a vertical line. Visualization: ./00_assets/fig_02-19.png
    return FieldElementPoint(this->a, this->b);
  }  
  
  FieldElement slope;
  // What is the value of slope at the moment?
  //It is NOT left undefined--it will be defined by the default constructor!
  if (this->x == other.x && this->y == other.y) {
    // p1 == p2, need some calculus to derive formula: (slope = 3x^2 + a) / 2y
    // Essentially this is the tangent line at P1. Visualization: ./00_assets/fig_02-18.png
    slope = (this->x.power(2) * 3 + this->a) / (this->y * 2);
  } else {
    // general case. Visualization: ./00_assets/fig_02-11.png
    slope = (this->y - other.y) / (this->x - other.x);
  }
  FieldElement x3 = FieldElement(slope * slope - this->x - other.x);
  FieldElement y3 = FieldElement(slope * (this->x - x3) - this->y);
  return FieldElementPoint(x3, y3, this->a, this->b);
  
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
  FieldElementPoint result = FieldElementPoint(this->a, this->b);
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
    ss << "FieldElementPoint(Infinity)_" << this->a.num() <<  "_" << this->b.num() << " FieldElement(" << this->a.prime() << ")";
  } else {
    ss << "FieldElementPoint(" << this->x.num() << ", " << this->y.num() << ")_"
       << this->a.num() <<  "_" << this->b.num() << " FieldElement(" << this->a.prime() << ")";    
  }
  return ss.str();
}

bool FieldElementPoint::infinity() {
  return this->infinity_;
}

const int512_t S256Element::s256Prime = (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f";

S256Element::S256Element(int512_t num): FieldElement(num, S256Element::s256Prime) {}

string S256Element::toString() {
  return FieldElement::toString(true);
}

const S256Element S256Point::a = S256Element(0);
const S256Element S256Point::b = S256Element(7);

S256Point::S256Point(S256Element x, S256Element y): FieldElementPoint(x, y, S256Point::a, S256Point::b) {}
S256Point::S256Point(): FieldElementPoint(S256Point::a, S256Point::b) {}