#include <iostream>
#include <stdexcept>
#include <math.h>

using namespace std;

class FieldElement {
  public:
    int num, prime;
    FieldElement(int,int);
    bool operator==(const FieldElement& other) const;
    FieldElement operator+(const FieldElement& other);
    FieldElement operator*(const FieldElement& other);
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

FieldElement FieldElement::operator+(const FieldElement& other)
{
  // ICYW: This is overloading, not overriding lol
  if (this->prime != other.prime) {
    throw std::invalid_argument("prime numbers are different");
  }
  return FieldElement((this->num + other.num) % this->prime, this->prime);
}

FieldElement FieldElement::operator*(const FieldElement& other)
{
  if (this->prime != other.prime) {
    throw std::invalid_argument("prime numbers are different");
  }
  return FieldElement((this->num * other.num) % this->prime, this->prime);
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

void testDivision() {
  FieldElement *a = new FieldElement(7, 19);
  FieldElement b = FieldElement(5, 19);
  cout << "Test division:\n"
       << (FieldElement(2, 19)/FieldElement(7, 19)).toString() << "\n"
       << ((*a)/b).toString() << endl;
  delete a;
}

void testPower() {
  FieldElement *a = new FieldElement(3, 13);
  FieldElement b = FieldElement(1, 13);
  cout << "Test power:\n"
      << ((*a).power(3) == b) << "\n"
      << (FieldElement(95, 97) * FieldElement(45, 97) * FieldElement(31, 97)).toString()
      << endl;
  delete a;
}

void testAddition() {
  FieldElement *a = new FieldElement(7,  13);
  FieldElement b = FieldElement(12, 13);
  FieldElement c = FieldElement(6,  13);
  // We can define an object in two ways.
  // The 1st way, heap memory is used, we need to take care of it ourselves
  // The 2nd way, stack memory is used, the object will be destoryed automatically. 
  cout << "Test addition:\n" << (*a + b == c) << endl;
  cout << (c * (*a)).toString() << endl;

  delete a;
}

int main() {
  testAddition();
  testPower();
  testDivision();
  cout << "Exercise 8" << endl;
  cout << (FieldElement(3, 31)/FieldElement(24, 31)).toString() << endl;
  cout << (FieldElement(17, 31).power(-3)).toString() << endl;
  cout << (FieldElement(4, 31).power(-4)*FieldElement(11, 31)).toString() << endl;
  return 0;
}