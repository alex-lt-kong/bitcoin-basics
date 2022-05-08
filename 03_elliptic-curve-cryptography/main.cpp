#include <iostream>
#include <stdexcept>
#include <boost/multiprecision/cpp_int.hpp>

#include "field-element-point.h"

using namespace std;
using namespace boost::multiprecision;

void testIfPointsOnCurve() {
  try {
    FieldElementPoint p2 = FieldElementPoint(
      FieldElement(17, 223), FieldElement(56, 223), FieldElement(0, 223), FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cerr << "p2: Invalid argument: " << ia.what() << endl;
  }

  try {
    FieldElementPoint p3 = FieldElementPoint(
      FieldElement(200, 223), FieldElement(119, 223), FieldElement(0, 223), FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cerr << "p3: Invalid argument: " << ia.what() << endl;
  }

  try {
    FieldElementPoint p4 = FieldElementPoint(
      FieldElement(1, 223), FieldElement(193, 223), FieldElement(0, 223), FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cerr << "p4: Invalid argument: " << ia.what() << endl;
  }
  
  try {
    FieldElementPoint p5 = FieldElementPoint(
      FieldElement(42, 223), FieldElement(99, 223), FieldElement(0, 223), FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cerr << "p5: Invalid argument: " << ia.what() << endl;
  }
}


void testFieldElementPointAddition() {
  cout << "testFieldElementPointAddition()" << endl;
  int prime = 223;
  
  FieldElement a = FieldElement(0, prime);
  FieldElement b = FieldElement(7, prime);

  FieldElement x1;
  FieldElement y1;
  FieldElement x2;
  FieldElement y2;

  x1 = FieldElement(192, prime);
  y1 = FieldElement(105, prime);
  x2 = FieldElement(17, prime);
  y2 = FieldElement(56, prime);
  FieldElementPoint p1 = FieldElementPoint(x1, y1, a, b);
  FieldElementPoint p2 = FieldElementPoint(x2, y2, a, b);
  cout << (p1 + p2).toString() << endl;

  x1 = FieldElement(170, prime);
  y1 = FieldElement(142, prime);
  x2 = FieldElement(60, prime);
  y2 = FieldElement(139, prime);
  cout << (FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b)).toString() << endl;

  x1 = FieldElement(47, prime);
  y1 = FieldElement(71, prime);
  x2 = FieldElement(17, prime);
  y2 = FieldElement(56, prime);
  cout << (FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b)).toString() << endl;

  x1 = FieldElement(143, prime);
  y1 = FieldElement(98, prime);
  x2 = FieldElement(76, prime);
  y2 = FieldElement(66, prime);
  cout << (FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b)).toString() << endl;

}

void testFieldElementPointScalarMultiplication() {
  cout << "testFieldElementPointScalarMultiplication()" << endl;
  int prime = 223;
  FieldElement a = FieldElement(0, prime);
  FieldElement b = FieldElement(7, prime);

  FieldElement x1;
  FieldElement y1;
  FieldElement x2;
  FieldElement y2;

  x1 = FieldElement(192, prime);
  y1 = FieldElement(105, prime);
  FieldElementPoint p1 = FieldElementPoint(x1, y1, a, b);
  cout << "(" << x1.toString() << ", " << y1.toString() << "): " << (p1 + p1).toString() << " == " << (p1 * 2).toString() << endl;

  x1 = FieldElement(143, prime);
  y1 = FieldElement(98, prime);
  p1 = FieldElementPoint(x1, y1, a, b);
  cout << "(" << x1.toString() << ", " << y1.toString() << "): " << (p1 + p1).toString() << " == " << (p1 * 2).toString() << endl;

  x1 = FieldElement(47, prime);
  y1 = FieldElement(71, prime);
  p1 = FieldElementPoint(x1, y1, a, b);
  cout << "(" << x1.toString() << ", " << y1.toString() << "): " << (p1+p1).toString() << " == " << (p1 * 2).toString() << endl;
  cout << "(" << x1.toString() << ", " << y1.toString() << "): " << (p1+p1+p1+p1).toString() << " == " << (p1 * 4).toString() << endl;
  cout << "(" << x1.toString() << ", " << y1.toString() << "): " << (p1+p1+p1+p1+p1+p1+p1+p1).toString() << " == " << (p1 * 8).toString() << endl;
  cout << "(" << x1.toString() << ", " << y1.toString() << "): " << (p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1).toString() << " == " << (p1 * 21).toString() << endl;

  x1 = FieldElement(15, prime);
  y1 = FieldElement(86, prime);
  p1 = FieldElementPoint(x1, y1, a, b);
  cout << (p1 * 7).toString() << endl;
  cout << (p1 + p1 + p1 + p1 + p1 + p1 + p1).toString() << endl;
  cout << (p1 * 8).toString() << endl;
  cout << (p1 + p1 + p1 + p1 + p1 + p1 + p1 + p1).toString() << endl;
}

int main() {
  testIfPointsOnCurve();
  cout << endl;
  testFieldElementPointAddition();
  cout << endl;
  testFieldElementPointScalarMultiplication();
  cout << endl;
  return 0;
}