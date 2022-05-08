#include <iostream>
#include "field-element.h"

using namespace std;
using namespace boost::multiprecision;

bool FieldElement::isPrimeNumber(int256_t input) {

    if (input < 2) { return false; }
    if (input == 2) { return true; }
    if (input % 2 == 0) { return false; }
    for (int256_t i = 3; (i*i)<=input; i+=2) {
        if(input % i == 0 ) { return false; }
    }
    return true;
}

FieldElement::FieldElement(int256_t num, int256_t prime) {
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

int256_t FieldElement::num() {
  return this->num_;
}

int256_t FieldElement::prime() {
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
  int256_t result = (this->num_ - other.num_) % this->prime_;
  if (result < 0) { result += this->prime_; }
  return FieldElement((int256_t)result, this->prime_);
}

FieldElement FieldElement::operator*(const FieldElement& other)
{
  if (this->prime_ != other.prime_) {
    throw std::invalid_argument("prime numbers are different");
  }
  return FieldElement((this->num_ * other.num_) % this->prime_, this->prime_);
}

FieldElement FieldElement::operator*(const int256_t other)
{
  // This implementation is inspired by FieldElementPoint FieldElementPoint::operator*(const int other)
  int256_t coef = other;
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
  int256_t tmp = FieldElement(other.num_, other.prime_).power(other.prime_-2).num_;
  return FieldElement(
    (this->num_ * tmp) % this->prime_,
    this->prime_
  );
}

int256_t FieldElement::modulusPower(int256_t exponent, int256_t modulus)
{
  int256_t n = exponent;
  int256_t result = 1;
  while (n < 0) { n += modulus - 1; };
  // C++ -7 % 3 = -1; python: -7 % 3 = 2;
  for (int256_t i = 0; i < n; i++) {
    result *= this->num_;
    result = result % modulus;
  }
  return result;
}

FieldElement FieldElement::power(int256_t exponent)
{
  return FieldElement(this->modulusPower(exponent, this->prime_), this->prime_);
}

string FieldElement::toString() {
  return this->num_.str() + " (" + this->prime_.str()  + ")";
}