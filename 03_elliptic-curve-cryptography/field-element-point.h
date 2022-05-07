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
  this->x = x == nullptr ? nullptr: new FieldElement(*x);
  this->y = y == nullptr ? nullptr: new FieldElement(*y);
  this->a = new FieldElement(*a);
  this->b = new FieldElement(*b);
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
  return this->a == other.a && this->b == other.b && this->x == other.x && this->y == other.y;
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

  if (*this == other && *(this->y) == 0) {
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
  FieldElement x3 = slope * slope - *(this->x) - *(other.x);
  FieldElement y3 = slope * (*(this->x) - x3) - *(this->y);
  return FieldElementPoint(&x3, &y3, this->a, this->b);
  
}

FieldElementPoint FieldElementPoint::operator*(const int other)
{
  FieldElementPoint fp =  FieldElementPoint(this->x, this->y, this->a, this->b);
  FieldElementPoint fp1 =  FieldElementPoint(this->x, this->y, this->a, this->b);
  //FieldElementPoint* fp = this;
  for (int i = 0; i < other; i++) {
    cout << "here!" << endl;
    fp = fp + fp1;
    cout << "here!" << endl;
  }
  return fp;
}

string FieldElementPoint::toString() {
  cout << "hello" << endl;
  string xNum = this->x != nullptr ? to_string(this->x->num) : "Infinity";
  string yNum = this->y != nullptr ? to_string(this->y->num) : "Infinity";
  return "FieldElementPoint(" + xNum + ", " + yNum + ")_"
                              + to_string(this->a->num) +  "_" + to_string(this->b->num)
                              + " FieldElement(" + to_string(this->a->prime) + ")";

}

#endif