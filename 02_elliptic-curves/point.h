#include <math.h>
#include <iostream>
#include <stdexcept>
#include <float.h>
#include <limits.h>

#ifndef Point_H
#define Point_H

using namespace std;

class Point {
  public:
    float a = 0;
    float b = 0;
    float x = 0;
    float y = 0;
    Point(float x, float y, float a, float b);
    bool operator==(const Point& other) const;
    Point operator+(const Point& other);
    string toString();
};

Point::Point(float x, float y, float a, float b) {
  this->a = a;
  this->b = b;
  this->x = x;
  this->y = y;
  
  if (this->x == FLT_MAX && this->y == FLT_MAX) { return; }

  if (pow(y, 2) != pow(x, 3) + a * x + b) {
    throw std::invalid_argument(
      "(" + to_string(x) + ", " + to_string(y) + ") is not on the curve"
    );
  }

}

bool Point::operator==(const Point& other) const
{
  ;
  // ICYW: This is overloading, not overriding
  return (
    this->a == other.a && this->b == other.b && 
    this->x == other.x && this->y == other.y
  );
}

Point Point::operator+(const Point& other)
{
  // ICYW: This is overloading, not overriding lol
  if (*this == other || this->y == 0) {
    return Point(FLT_MAX, FLT_MAX, this->a, this->b);
  }
  if (this->a != other.a || this->b != other.b) {
    throw std::invalid_argument("Two points are not on the same curve");
  }
  // FLT_MAX represents infinity
  if (this->x == FLT_MAX) { return other; }
  if (other.x == FLT_MAX) { return *this; }
  if (this->x == other.x && this->y != other.y) {
    return Point(FLT_MAX, FLT_MAX, this->a, this->b);//point at infinity
  }

  float slope = 0;
  if (this->x == other.x && this->y == other.y) {
    // P1 == P2, need some calculus to derive this formula
    slope = (3 * this->x * this->x + this->a) / (this->y * 2);
  } else {
    // general case
    slope = (other.y - this->y) / (other.x - this->x);
  }
  float x3 = slope * slope - this->x - other.x;
  float y3 = slope * (this->x - x3) - this->y;
  return Point(x3, y3, this->a, this->b);
}

string Point::toString() {
  return "Point(" + (this->x == FLT_MAX ? "Inf" : to_string(this->x)) + ", "
                  + (this->y == FLT_MAX ? "Inf" : to_string(this->y)) + ")_"
                  + to_string(this->a) + "_" + to_string(this->b);
}

#endif