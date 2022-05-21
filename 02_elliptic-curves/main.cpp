#include <iostream>
#include "point.h"

using namespace std;


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
  cout << "p1: "<< p1.to_string() << endl;
  try{
    Point p2 = Point(-1, -2, 5, 7); }
  catch (const invalid_argument& ia) {
    cerr << "p2: Invalid argument: " << ia.what() << '\n';
  }
  try{
    Point p3 = Point(2, 4, 5, 7); }
  catch (const invalid_argument& ia) {
    cerr << "p3: Invalid argument: " << ia.what() << '\n';
  }
  try{
    Point p4 = Point(18, 77, 5, 7); }
  catch (const invalid_argument& ia) {
    cerr << "p4: Invalid argument: " << ia.what() << '\n';
  }
  try{
    Point p5 = Point(5, 7, 5, 7); }
  catch (const invalid_argument& ia) {
    cerr << "p5: Invalid argument: " << ia.what() << '\n';
  }
  testPointAdditionInfinity();
  testPointAddinitionDifferentXs();
  testPointAddinitionSamePoint();
  return 0;
}