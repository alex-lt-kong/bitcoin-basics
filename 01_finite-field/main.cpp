#include <iostream>
#include <stdexcept>
using namespace std;

class FieldElement {
  public:
    int num, prime;
    FieldElement(int,int);
    bool operator==(const FieldElement& other) const;
    FieldElement operator+(const FieldElement& other);
};

FieldElement::FieldElement(int num, int prime) {
  if (num >= prime || num < 0) {
    throw std::invalid_argument(
      "invalid num: " + std::to_string(num) + 
      ". Either it is negative or greater than prime"
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

int main() {
  FieldElement a = FieldElement(7,  13);
  FieldElement b = FieldElement(12, 13);
  FieldElement c = FieldElement(6,  13);
  cout << (a + b == c) << endl;

}