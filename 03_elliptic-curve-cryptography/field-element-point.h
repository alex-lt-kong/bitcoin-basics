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
    FieldElement a;
    FieldElement b;
    FieldElement x;
    FieldElement y;
    // We need to have a default constructor FieldElement() defined to make this work.
    bool infinity = false;
  public:
    FieldElementPoint(FieldElement x, FieldElement y, FieldElement a, FieldElement b);
    FieldElementPoint(bool infinity, FieldElement a, FieldElement b);
    FieldElementPoint(const FieldElementPoint& point);
    ~FieldElementPoint();
    bool operator==(const FieldElementPoint& other) const;
    FieldElementPoint operator+(const FieldElementPoint& other);
    FieldElementPoint operator*(const int coef);
    string toString();
};

FieldElementPoint::FieldElementPoint(FieldElement x, FieldElement y, FieldElement a, FieldElement b) {  
  // seems here we need to point the internal pointers to new objects--if we point to the same objects 
  // as x, y, a, b, if these objects get deleted by some other function, the this->x, this->y, this->a, this->b,
  // will point to nothing. 
  this->x = FieldElement(x.num, x.prime);
  this->y = FieldElement(y.num, y.prime);
  this->a = FieldElement(a.num, a.prime);
  this->b = FieldElement(b.num, b.prime);
  
  if (this->y.power(2) != this->x.power(3) + (this->a * this->x) + this->b) {    
    throw invalid_argument("Point (" + this->x.toString() + ", " + this->y.toString() +") not on the curve");
  }
}

FieldElementPoint::FieldElementPoint(bool infinity, FieldElement a, FieldElement b) {
  this->infinity = true;
  this->a = FieldElement(a.num, a.prime);
  this->b = FieldElement(b.num, b.prime);  
}

FieldElementPoint::FieldElementPoint(const FieldElementPoint& point) {
  this->infinity = point.infinity;
  if (this->infinity == false) {
    this->x = FieldElement(point.x.num, point.x.prime);
    this->y = FieldElement(point.y.num, point.y.prime);
  }
  this->a = FieldElement(point.a.num, point.a.prime);
  this->b = FieldElement(point.b.num, point.b.prime);  
}

FieldElementPoint::~FieldElementPoint() {
}


bool FieldElementPoint::operator==(const FieldElementPoint& other) const
{
  // ICYW: This is overloading, not overriding. An interesting point to note is that a,b,x,y are private members
  // of other, but still we can access them.
  bool xyEqual = false;
  if (this->infinity == true && other.infinity == true) { xyEqual = true; }
  else if (this->infinity != other.infinity) { xyEqual = false; }
  else if (this->infinity == false && other.infinity == false) { xyEqual = (this->x == other.x && this->y == other.y); }
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
  if (this->infinity) {
    // this->x == nullptr means this point is at infinity
    return other; 
  }
  if (other.infinity) {
    // other.x == nullptr means the other point is at infinity
    return *this; 
  }

  if (*this == other && this->y == FieldElement(0, this->y.prime)) {
    // It means p1 == p2 and tangent is a vertical line.
    return FieldElementPoint(true, this->a, this->b);
  }

  
  if (this->x == other.x &&  this->y != other.y) {
    // FieldElementPoint at infinity
    return FieldElementPoint(true, this->a, this->b);
  }
  
  FieldElement slope = FieldElement(0, this->x.prime);
  if (this->x == other.x && this->y == other.y) {
    // P1 == P2, need some calculus to derive formula: slope = 3x^2 + a / 2y
    // Essentially this is the tangent line at P1
    slope = (this->x.power(2) * 3 + this->a) / (this->y * 2);
  } else {
    // general case
    slope = (this->y - other.y) / (this->x - other.x);
  }
  FieldElement x3 = FieldElement(slope * slope - this->x - other.x);
  FieldElement y3 = FieldElement(slope * (this->x - x3) - this->y);
  return FieldElementPoint(x3, y3, this->a, this->b);
  
}

FieldElementPoint FieldElementPoint::operator*(const int other)
{
  FieldElementPoint fp = FieldElementPoint(true, this->a, this->b);
  for (int i = 0; i < other; i++) {
    fp = FieldElementPoint(*this + fp);
  }
  return fp;
}

string FieldElementPoint::toString() {
  if (this->infinity) {
    return "FieldElementPoint(Infinity)_" + 
            to_string(this->a.num) +  "_" + to_string(this->b.num) +
            " FieldElement(" + to_string(this->a.prime) + ")";
  } else {
  return "FieldElementPoint(" + to_string(this->x.num) + ", " + to_string(this->y.num) + ")_"
                              + to_string(this->a.num) +  "_" + to_string(this->b.num)
                              + " FieldElement(" + to_string(this->a.prime) + ")";
  }

}

#endif