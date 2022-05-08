#ifndef FieldElement_H
#define FieldElement_H

#include <iostream>

using namespace std;

// Represent an integer in a finite field. Given the application of the field, it is defined by:
// ** a prime number as order (i.e., size) of the field
// ** a non-negative integer an element of the field. The integer has to be smaller than order of the field.
class FieldElement {
  // Note that FieldElement is a number in a given field, infinity is not a valid
  // number in that field so we don't need to have a design which handles infinity
  // here. Infinity happens only at FieldElementPoint level.
  private:
    int num_ = 0, prime_ = 2;
    // We make the initial value valid so that we can have an empty default constructor.
    // Note that in Python we prepend underscores, in C++ it is recommended to append underscores as variables
    // start with an underscore or double underscore are reserved for the C++ implementers
    bool isPrimeNumber(int);
  public:    
    FieldElement(int, int);
    FieldElement();    
    bool operator==(const FieldElement& other) const;
    bool operator!=(const FieldElement& other) const;
    FieldElement operator+(const FieldElement& other);
    FieldElement operator-(const FieldElement& other);
    FieldElement operator*(const FieldElement& other);
    FieldElement operator*(const int other);
    FieldElement operator/(const FieldElement& other);
    FieldElement power(int exponent);
    int modulusPower(int exponent, int modulus);
    string toString();
    int num();
    int prime();
};

bool FieldElement::isPrimeNumber(int input) {

    if (input < 2) { return false; }
    if (input == 2) { return true; }
    if (input % 2 == 0) { return false; }
    for (long long int i = 3; (i*i)<=input; i+=2) {
        if(input % i == 0 ) { return false; }
    }
    return true;
}

FieldElement::FieldElement(int num, int prime) {
  if (num >= prime || num < 0) {
    throw invalid_argument("invalid num [" + to_string(num) + "]: negative or greater than prime");
  }
  if (!isPrimeNumber(prime)) {
    throw invalid_argument("invalid prime [" + to_string(prime) + "]: not a prime number");
  }
  this->num_ = num;
  this->prime_ = prime;
}

FieldElement::FieldElement() {
  // This is needed if we want to declare an object without defining it
  // It will be defined with this default constructor automatically.
  
  // FieldElement(0, 1); seems in C++ we can't call another constructor in a constructor...  
}

int FieldElement::num() {
  return this->num_;
}

int FieldElement::prime() {
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
  int result = (this->num_ - other.num_) % this->prime_;
  if (result < 0) { result += this->prime_; }
  return FieldElement(result, this->prime_);
}

FieldElement FieldElement::operator*(const FieldElement& other)
{
  if (this->prime_ != other.prime_) {
    throw std::invalid_argument("prime numbers are different");
  }
  return FieldElement((this->num_ * other.num_) % this->prime_, this->prime_);
}

FieldElement FieldElement::operator*(const int other)
{
  FieldElement fe = FieldElement(*this);
  for (int i = 0; i < other - 1; i ++) {
    fe = fe + *this;
  }
  return fe;
}

FieldElement FieldElement::operator/(const FieldElement& other)
{
  if (this->prime_ != other.prime_) {
    throw std::invalid_argument("prime numbers are different");
  }
  int tmp = FieldElement(other.num_, other.prime_).power(other.prime_-2).num_;
  return FieldElement(
    (this->num_ * tmp) % this->prime_,
    this->prime_
  );
}

int FieldElement::modulusPower(int exponent, int modulus)
{
  int n = exponent;
  int result = 1;
  while (n < 0) { n += modulus - 1; };
  // C++ -7 % 3 = -1; python: -7 % 3 = 2;
  for (int i = 0; i < n; i++) {
    result *= this->num_;
    result = result % modulus;
  }
  return result;
}

FieldElement FieldElement::power(int exponent)
{
  return FieldElement(this->modulusPower(exponent, this->prime_), this->prime_);
}

string FieldElement::toString() {
  return to_string(this->num_) + " (" +to_string(this->prime_)  + ")";
}

#endif