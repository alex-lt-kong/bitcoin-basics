#ifndef FieldElementPoint_H
#define FieldElementPoint_H

#include "../01_finite-field/field-element.h"

using namespace std;

// Represent a point consists of two finite field elements, given the context of its application, it is defined by:
// ** x, y: two FieldElements representing the coordinates of the point. Not passing x and y to constructor means the point
// is at infinity
// ** a, b: as defined in elliptic curve's canonical form y^2 = x^3 + ax + b
class FieldElementPoint {

  private:
    // The canonical form of an elliptic curve is y^2 = x^3 + ax + b, thus the following a, b, x, y for a point
    // In the book's implementation, a and b are also FieldElements--this seems necessary--if b is not in the same finite
    // field, the addition between two numbers in different fields seems undefined.
    FieldElement a;
    FieldElement b;
    FieldElement x;
    FieldElement y;
    // We need to have a default constructor FieldElement() defined to make this work.
    bool infinity = false;
  public:
    FieldElementPoint(FieldElement x, FieldElement y, FieldElement a, FieldElement b);
    FieldElementPoint(FieldElement a, FieldElement b);
    ~FieldElementPoint();
    bool operator==(const FieldElementPoint& other) const;
    FieldElementPoint operator+(const FieldElementPoint& other);
    FieldElementPoint operator*(const int coef);
    string toString();
};

#endif