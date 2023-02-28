#include <iostream>
#include <math.h>
#include <stdexcept>
#include <float.h>
#include <limits.h>

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
    string to_string();
};

Point::Point(float x, float y, float a, float b) {
  this->a = a;
  this->b = b;
  this->x = x;
  this->y = y;
  
  if (this->x == FLT_MAX && this->y == FLT_MAX) { return; }

  if (pow(y, 2) != pow(x, 3) + a * x + b) {
    throw invalid_argument(
      "(" + std::to_string(x) + ", " + std::to_string(y) + ") is not on the curve"
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

string Point::to_string() {
  return "Point(" + (this->x == FLT_MAX ? "Inf" : std::to_string(this->x)) + ", "
                  + (this->y == FLT_MAX ? "Inf" : std::to_string(this->y)) + ")_"
                  + std::to_string(this->a) + "_" + std::to_string(this->b);
}


void testPointAdditionInfinity() {
  Point p1 = Point(-1, -1, 5, 7);
  Point p2 = Point(-1, 1, 5, 7);
  Point pInf = Point(FLT_MAX, FLT_MAX, 5, 7);
  cout << "testPointAdditionInfinity:\n"
       << (p1 + pInf).to_string() << "\n"
       << (p2 + pInf).to_string() << "\n"
       << (p1 + p2).to_string() << endl;
  
}

void testPointAddinitionDifferentXs() {
  cout << "testPointAdditionInfinity:\n"
       << (Point(2, 5, 5, 7) + Point(-1, -1, 5, 7)).to_string()
       << endl;
}


void testPointAddinitionSamePoint() {
  cout << "testPointAddinitionSamePoint:\n"
       << (Point(-1, -1, 5, 7) + Point(-1, -1, 5, 7)).to_string()
       << endl;
}

int main() {
  Point p1 = Point(-1, -1, 5, 7);
  cout << "p1: " << p1.to_string() << endl;
  try{
    Point p2 = Point(-1, -2, 5, 7); 
    printf("%s\n", p2.to_string().c_str());
  }
  catch (const invalid_argument& ia) {
    cerr << "p2: Invalid argument: " << ia.what() << '\n';
  }
  try{
    Point p3 = Point(2, 4, 5, 7);
    printf("%s\n", p3.to_string().c_str());
  }    
  catch (const invalid_argument& ia) {
    cerr << "p3: Invalid argument: " << ia.what() << '\n';
  }
  try{
    Point p4 = Point(18, 77, 5, 7); 
    printf("%s\n", p4.to_string().c_str());
  }
  catch (const invalid_argument& ia) {
    cerr << "p4: Invalid argument: " << ia.what() << '\n';
  }
  try{
    Point p5 = Point(5, 7, 5, 7);
    printf("%s\n", p5.to_string().c_str());
  }
  catch (const invalid_argument& ia) {
    cerr << "p5: Invalid argument: " << ia.what() << '\n';
  }
  testPointAdditionInfinity();
  testPointAddinitionDifferentXs();
  testPointAddinitionSamePoint();
  return 0;
}