#ifndef ECC_H
#define ECC_H

#include <boost/multiprecision/cpp_int.hpp>
#include "sha256.h"

using namespace std;
using namespace boost::multiprecision;

// Represent an integer in a finite field. Given the application of the field in this context, it is defined by:
// ** a prime number as order (i.e., size) of the field
// ** a non-negative integer an element of the field. The integer has to be smaller than order of the field.
class FieldElement {
// Note that FieldElement is a number in a given field, infinity is not a valid
// number in that field so we don't need to have a design which handles infinity
// here. Infinity happens only at FieldElementPoint level.
protected:
  // We make the initial value valid so that we can have an empty default constructor.
  // Note that in Python we prepend underscores, in C++ it is recommended to append underscores as variables
  // start with an underscore or double underscore are reserved for the C++ implementers
  int512_t num_ = 0, prime_ = 2;
  bool isPrimeNumber(int512_t);
public:    
  FieldElement(int512_t num, int512_t prime);
  FieldElement();    
  bool operator==(const FieldElement& other) const;
  bool operator!=(const FieldElement& other) const;
  FieldElement operator+(const FieldElement& other);
  FieldElement operator-(const FieldElement& other);
  FieldElement operator*(const FieldElement& other);
  FieldElement operator*(const int512_t other);
  FieldElement operator/(const FieldElement& other);
  FieldElement power(int512_t exponent);
  int512_t modulusPower(int512_t exponent, int512_t modulus);
  string toString(bool inHex=false);
  int512_t num();
  int512_t prime();
};


// Represent a point consists of two finite field elements, given the context of its application, it is defined by:
// ** x, y: two FieldElements representing the coordinates of the point. Not passing x and y to constructor means the point
// is at infinity
// ** a, b: as defined in elliptic curve's canonical form y^2 = x^3 + ax + b
class FieldElementPoint {

protected:
  // The canonical form of an elliptic curve is y^2 = x^3 + ax + b, thus the following a, b, x, y for a point
  // In the book's implementation, a and b are also FieldElements--this seems necessary--if b is not in the same finite
  // field, the addition between two numbers in different fields seems undefined.
  FieldElement a_;
  FieldElement b_;
  FieldElement x_;
  FieldElement y_;
  // We need to have a default constructor FieldElement() defined to make this work.
  bool infinity_ = false;
public:
  FieldElementPoint(FieldElement x, FieldElement y, FieldElement a, FieldElement b);
  FieldElementPoint(FieldElement a, FieldElement b);
  ~FieldElementPoint();
  bool operator==(const FieldElementPoint& other) const;
  FieldElementPoint operator+(const FieldElementPoint& other);
  FieldElementPoint operator+=(const FieldElementPoint& other);
  FieldElementPoint operator*(const int512_t coef);
  string toString(bool inHex=false);
  bool infinity();
  FieldElement x();
  FieldElement y();
};


class S256Element : public FieldElement
{
private:
  static const int512_t s256Prime_; // the value is assigned in ecc.cpp
public:
  S256Element(int512_t num);
  string toString();
  int512_t s256Prime();
};


class Signature {
private:
  int512_t r_ = -1;
  int512_t s_ = -1;
public:
  Signature(int512_t r, int512_t s);
  string toString();
  int512_t r();
  int512_t s();
};


class S256Point : public FieldElementPoint
{
private:
  static const S256Element a_;
  static const S256Element b_;
  static const int512_t order_;
public:
  S256Point(S256Element x, S256Element y);
  S256Point();
  bool verify(int512_t msgHash, Signature sig);
  int512_t s256Prime();
  S256Point operator+(const S256Point other);
  S256Point operator*(const int512_t coef);
  int512_t order();
  S256Element a();
  S256Element b();
  string toString();
};

// The generator point of secp256k1
extern S256Point G;


class ECDSAPrivateKey {
protected:
  unsigned char* secretBytes_ = nullptr;
  unsigned short int secretLen_ = SHA256_HASH_SIZE;
  int512_t secret_ = -1;
  S256Point p_;
  int512_t getDeterministicK(unsigned char* msgHashBytes, unsigned short int msgHashLen);
public:
  ECDSAPrivateKey(unsigned char* secretBytes, unsigned short int secretLen);
  ~ECDSAPrivateKey();
  string toString();
  Signature sign(unsigned char* msgHashBytes, unsigned short int msgHashLen);
  
};

#endif
