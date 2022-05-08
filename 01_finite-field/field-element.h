#ifndef FieldElement_H
#define FieldElement_H

using namespace std;

// Represent an integer in a finite field. Given the application of the field in this context, it is defined by:
// ** a prime number as order (i.e., size) of the field
// ** a non-negative integer an element of the field. The integer has to be smaller than order of the field.
class FieldElement {
  // Note that FieldElement is a number in a given field, infinity is not a valid
  // number in that field so we don't need to have a design which handles infinity
  // here. Infinity happens only at FieldElementPoint level.
  private:
    // We make the initial value valid so that we can have an empty default constructor.
    // Note that in Python we prepend underscores, in C++ it is recommended to append underscores as variables
    // start with an underscore or double underscore are reserved for the C++ implementers
    int num_ = 0, prime_ = 2;
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

#endif