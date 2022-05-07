#ifndef FieldElement_H
#define FieldElement_H

#include <iostream>

using namespace std;

class FieldElement {
  public:
    int num, prime;
    FieldElement(int, int);
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
};

FieldElement::FieldElement(int num, int prime) {
  if (num >= prime || num < 0) {
    throw std::invalid_argument(
      "invalid num: " + to_string(num) + ". Either it is negative or greater than prime"
    );
  }
  this->num = num;
  this->prime = prime;
}

bool FieldElement::operator==(const FieldElement& other) const
{
  // ICYW: This is overloading, not overriding lol
  return this->prime == other.prime && this->num == other.num;
}

bool FieldElement::operator!=(const FieldElement& other) const
{
  // ICYW: This is overloading, not overriding lol
  return !(*this == other);
}

FieldElement FieldElement::operator+(const FieldElement& other)
{
  // ICYW: This is overloading, not overriding lol
  if (this->prime != other.prime) {
    throw std::invalid_argument("prime numbers are different");
  }
  return FieldElement((this->num + other.num) % this->prime, this->prime);
}

FieldElement FieldElement::operator-(const FieldElement& other)
{
  // ICYW: This is overloading, not overriding lol
  if (this->prime != other.prime) {
    throw std::invalid_argument("prime numbers are different");
  }
  int result = (this->num - other.num) % this->prime;
  if (result < 0) { result += this->prime; }
  return FieldElement(result, this->prime);
}

FieldElement FieldElement::operator*(const FieldElement& other)
{
  if (this->prime != other.prime) {
    throw std::invalid_argument("prime numbers are different");
  }
  return FieldElement((this->num * other.num) % this->prime, this->prime);
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
  if (this->prime != other.prime) {
    throw std::invalid_argument("prime numbers are different");
  }
  int tmp = FieldElement(other.num, other.prime).power(other.prime-2).num;
  return FieldElement(
    (this->num * tmp) % this->prime,
    this->prime
  );
}

int FieldElement::modulusPower(int exponent, int modulus)
{
  int n = exponent;
  int result = 1;
  while (n < 0) { n += modulus - 1; };
  // C++ -7 % 3 = -1; python: -7 % 3 = 2;
  for (int i = 0; i < n; i++) {
    result *= this->num;
    result = result % modulus;
  }
  return result;
}

FieldElement FieldElement::power(int exponent)
{
  return FieldElement(this->modulusPower(exponent, this->prime), this->prime);
}

string FieldElement::toString() {
  return to_string(this->num) + " (" +to_string(this->prime)  + ")";
}

#endif