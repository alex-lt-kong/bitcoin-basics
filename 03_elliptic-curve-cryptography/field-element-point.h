#include <math.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <float.h>
#include <limits.h>
#include "../01_finite-field/field-element.h"

#ifndef FieldElementPoint_H
#define FieldElementPoint_H

using namespace std;

class FieldElementPoint {

  private:
    // The canonical form of an elliptic curve is y^2 = x^3 + ax + b, thus the following a, b, x, y for a point
    // In the book's implementation, a and b are also FieldElements. However, it seems to me that b does not
    // have to be a FieldElement though.
    FieldElement* a = nullptr;
    FieldElement* b = nullptr;
    FieldElement* x = nullptr;
    FieldElement* y = nullptr;
  public:
    FieldElementPoint(FieldElement* x, FieldElement* y, FieldElement* a, FieldElement* b);
    ~FieldElementPoint();
    bool operator==(const FieldElementPoint& other) const;
    FieldElementPoint operator+(const FieldElementPoint& other);
    FieldElementPoint operator*(const int coef);
    string toString();
};

FieldElementPoint::FieldElementPoint(FieldElement* x, FieldElement* y, FieldElement* a, FieldElement* b) {  
  // seems here we need to point the internal pointers to new objects--if we point to the same objects 
  // as x, y, a, b, if these objects get deleted by some other function, the this->x, this->y, this->a, this->b,
  // will point to nothing. 
  this->x = x == nullptr ? nullptr: new FieldElement(x->num, x->prime);
  this->y = y == nullptr ? nullptr: new FieldElement(y->num, y->prime);
  if (a == nullptr || b == nullptr) { throw std::invalid_argument("parameters a or b cannot be nullptr"); }
  this->a = new FieldElement(a->num, a->prime);
  this->b = new FieldElement(b->num, b->prime);
  if (x == nullptr && y == nullptr) {
    // we only check if (x, y) is at the curve if the point is not at infinity
    // x == nullptr or y == nullptr means the point is at infinity.
    return;
  }
  
  if (this->y->power(2) != this->x->power(3) + (*(this->a) * *(this->x)) + *(this->b)) {    
    throw std::invalid_argument(      
      "Point (" + this->x->toString() + ", " + this->y->toString() +") not on the curve"
    );
  }
}

FieldElementPoint::~FieldElementPoint() {  
  delete this->x;
  delete this->y;
  delete this->a;
  delete this->b;
}


bool FieldElementPoint::operator==(const FieldElementPoint& other) const
{
  // ICYW: This is overloading, not overriding
  bool xyEqual = false;
  if (this->x == nullptr && other.x == nullptr && this->y == nullptr && other.y == nullptr) { xyEqual = true; }
  else if (this->x == nullptr || other.x == nullptr || this->y == nullptr || other.y == nullptr) { xyEqual = false; }
  else { xyEqual = (this->x == other.x && this->y == other.y); }
  return this->a == other.a && this->b == other.b && xyEqual;
}

FieldElementPoint FieldElementPoint::operator+(const FieldElementPoint& other)
{  
  // ICYW: This is overloading, not overriding lol
  if (*(this->a) != *(other.a) || *(this->b) != *(other.b)) {
    throw std::invalid_argument("Two FieldElementPoints are not on the same curve");
  }
  if (this->x == nullptr) {
    // this->x == nullptr means this point is at infinity
    return other; 
  }
  if (other.x == nullptr) {
    // other.x == nullptr means the other point is at infinity
    return *this; 
  }

  if (*this == other && *(this->y) == FieldElement(0, this->y->prime)) {
    // It means p1 == p2 and tangent is a vertical line.
    return FieldElementPoint(nullptr, nullptr, this->a, this->b);
  }

  
  if (*(this->x) == *(other.x) && *(this->y) != *(other.y)) {
    // FieldElementPoint at infinity
    return FieldElementPoint(nullptr, nullptr, this->a, this->b);
  }
  
  FieldElement slope = FieldElement(0, this->x->prime);
  if (*(this->x) == *(other.x) && *(this->y) == *(other.y)) {
    // P1 == P2, need some calculus to derive formula: slope = 3x^2 + a / 2y
    // Essentially this is the tangent line at P1
    slope = ((this->x)->power(2) * 3 + *(this->a)) / (*(this->y) * 2);
  } else {
    // general case
    slope = (*(other.y) - *(this->y)) / (*(other.x) - *(this->x));
  }
  FieldElement x3 = FieldElement(slope * slope - *(this->x) - *(other.x));
  FieldElement y3 = FieldElement(slope * (*(this->x) - x3) - *(this->y));
  return FieldElementPoint(&x3, &y3, this->a, this->b);
  
}

FieldElementPoint FieldElementPoint::operator*(const int other)
{
  FieldElementPoint* fp = new FieldElementPoint(this->x, this->y, this->a, this->b);
  for (int i = 0; i < other - 1; i++) {
    FieldElementPoint* tmp = fp;
    fp = new FieldElementPoint(*this + *fp);
    delete tmp;
    // https://stackoverflow.com/questions/8763398/why-is-it-illegal-to-take-the-address-of-an-rvalue-temporary
  }
  FieldElementPoint result = FieldElementPoint(fp->x, fp->y, fp->a, fp->b);
  delete fp; // Note that we don't return *fp, but instead we create a reference result and return the reference.
  // This appears to me that by doing this we avoid memory leakage. What if we simply declare fp as a reference
  // instead of a pointer? No, that doesn't seem to work, compiler complains "free(): double free detected in tcache 2"
  return result;
  // You should never return "this" here--no matter what a user does, the result
  // should not change operand's own value.
}

string FieldElementPoint::toString() {
  string xNum = this->x != nullptr ? to_string(this->x->num) : "Infinity";
  string yNum = this->y != nullptr ? to_string(this->y->num) : "Infinity";
  return "FieldElementPoint(" + xNum + ", " + yNum + ")_"
                              + to_string(this->a->num) +  "_" + to_string(this->b->num)
                              + " FieldElement(" + to_string(this->a->prime) + ")";

}

#endif