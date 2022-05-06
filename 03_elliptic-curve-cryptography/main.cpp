#include <float.h>
#include <iostream>
#include <stdexcept>
#include <limits.h>
#include <math.h>

#include "field-element-point.h"

using namespace std;

void testIfPointsOnCurve() {
  try {
    FieldElementPoint p2 = FieldElementPoint(
      new FieldElement(17, 223), new FieldElement(56, 223), new FieldElement(0, 223), new FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cerr << "p2: Invalid argument: " << ia.what() << endl;
  }

  try {
    FieldElementPoint p3 = FieldElementPoint(
      new FieldElement(200, 223), new FieldElement(119, 223), new FieldElement(0, 223), new FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cerr << "p3: Invalid argument: " << ia.what() << endl;
  }

  try {
    FieldElementPoint p4 = FieldElementPoint(
      new FieldElement(1, 223), new FieldElement(193, 223), new FieldElement(0, 223), new FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cerr << "p4: Invalid argument: " << ia.what() << endl;
  }
  
  try {
    FieldElementPoint p5 = FieldElementPoint(
      new FieldElement(42, 223), new FieldElement(99, 223), new FieldElement(0, 223), new FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cerr << "p5: Invalid argument: " << ia.what() << endl;
  }
}

void defineFieldElementPoint(FieldElementPoint **p) {
  // A fancy way to initialize a pointer by reference
  *p = new FieldElementPoint(
    new FieldElement(192, 223), new FieldElement(105, 223), new FieldElement(0, 223), new FieldElement(7, 223)
  );
}

void testFieldElementPointAddition() {
  cout << "testFieldElementPointAddition()" << endl;
  int prime = 223;
  FieldElement* a = new FieldElement(0, prime);
  FieldElement* b = new FieldElement(7, prime);
  FieldElement* x1 = new FieldElement(192, prime);
  FieldElement* y1 = new FieldElement(105, prime);
  FieldElement* x2 = new FieldElement(17, prime);
  FieldElement* y2 = new FieldElement(56, prime);

  FieldElementPoint p1 = FieldElementPoint(x1, y1, a, b);
  FieldElementPoint p2 = FieldElementPoint(x2, y2, a, b);
  cout << (p1 + p2).toString() << endl;
}

int main() {
  FieldElementPoint* p0;
  defineFieldElementPoint(&p0);  
  cout << p0->toString() << endl;
  testIfPointsOnCurve();
  testFieldElementPointAddition();
  return 0;
}