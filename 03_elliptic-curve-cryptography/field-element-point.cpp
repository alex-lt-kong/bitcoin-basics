#include <math.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <float.h>
#include <limits.h>
#include "field-element-point.h"

using namespace std;

FieldElementPoint::FieldElementPoint(FieldElement x, FieldElement y, FieldElement a, FieldElement b) {  
  // seems here we need to point the internal pointers to new objects--if we point to the same objects 
  // as x, y, a, b, if these objects get deleted by some other function, the this->x, this->y, this->a, this->b,
  // will point to nothing. 
  this->x = FieldElement(x.num(), x.prime());
  this->y = FieldElement(y.num(), y.prime());
  this->a = FieldElement(a.num(), a.prime());
  this->b = FieldElement(b.num(), b.prime());
  
  if (this->y.power(2) != this->x.power(3) + (this->a * this->x) + this->b) {    
    throw invalid_argument("Point (" + this->x.toString() + ", " + this->y.toString() +") not on the curve");
  }
}


// Omitting x and y and passing only a and b in y^2 = x^3 + ax + b means this FieldElementPoint is point at infinity
FieldElementPoint::FieldElementPoint(FieldElement a, FieldElement b) {
  this->infinity = true;
  this->a = FieldElement(a.num(), a.prime());
  this->b = FieldElement(b.num(), b.prime());  
}

FieldElementPoint::~FieldElementPoint() {}

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

  // Point at infinity is defined as I where point A + I = A. Visualization: ./00_assets/fig_02-15.png
  if (this->infinity) { return other; }
  if (other.infinity) { return *this; }
  // meaning that A + (-A) = I. Visualization: ./00_assets/fig_02-15.png
  if (this->x == other.x &&  this->y != other.y) {    
    return FieldElementPoint(this->a, this->b);
  }

  if (*this == other && this->y == FieldElement(0, this->y.prime())) {
    // It means p1 == p2 and tangent is a vertical line. Visualization: ./00_assets/fig_02-19.png
    return FieldElementPoint(this->a, this->b);
  }  
  
  FieldElement slope;
  // What is the value of slope at the moment?
  //It is NOT left undefined--it will be defined by the default constructor!
  if (this->x == other.x && this->y == other.y) {
    // p1 == p2, need some calculus to derive formula: (slope = 3x^2 + a) / 2y
    // Essentially this is the tangent line at P1. Visualization: ./00_assets/fig_02-18.png
    slope = (this->x.power(2) * 3 + this->a) / (this->y * 2);
  } else {
    // general case. Visualization: ./00_assets/fig_02-11.png
    slope = (this->y - other.y) / (this->x - other.x);
  }
  FieldElement x3 = FieldElement(slope * slope - this->x - other.x);
  FieldElement y3 = FieldElement(slope * (this->x - x3) - this->y);
  return FieldElementPoint(x3, y3, this->a, this->b);
  
}

FieldElementPoint FieldElementPoint::operator*(const int other)
{
  // The current implementation is called "binary expansion". It reduces the time complexity from O(n) to Olog(n).
  // Essentially, we scan coef bit by bit and add the number to result if a bit is 1.
  int coef = other;
  FieldElementPoint result = FieldElementPoint(this->a, this->b);
  FieldElementPoint curr = FieldElementPoint(*this);
  /*
    for (int i = 0; i < other; i++) {
      result = FieldElementPoint(*this + result);
    }
  */
  while (coef > 0) {
    if (coef & 1) { // &: bitwise AND, check if the rightmost bit of coef == 1
      result = result + curr;
    }
    curr = curr + curr;
    // Can we use curr = curr * 2? NO! Since this is exactly where multiplication is defined! It causes a stack overflow
    // if we use * in FieldElementPoint::operator*()
    coef >>= 1; 
    // >>: right shift the bits of the first operand by the second operand number of places.
    // 14:       0000 1110
    // 14 >> 1:  0000 0111 = 7
    // 14 >> 2:  0000 0011 = 3
  }
    
  return result;
}

string FieldElementPoint::toString() {
  if (this->infinity) {
    return "FieldElementPoint(Infinity)_" + 
            to_string(this->a.num()) +  "_" + to_string(this->b.num()) +
            " FieldElement(" + to_string(this->a.prime()) + ")";
  } else {
  return "FieldElementPoint(" + to_string(this->x.num()) + ", " + to_string(this->y.num()) + ")_"
                              + to_string(this->a.num()) +  "_" + to_string(this->b.num())
                              + " FieldElement(" + to_string(this->a.prime()) + ")";
  }

}