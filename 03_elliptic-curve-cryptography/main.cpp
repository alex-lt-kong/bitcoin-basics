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

  FieldElement* x1 = nullptr;
  FieldElement* y1 = nullptr;
  FieldElement* x2 = nullptr;
  FieldElement* y2 = nullptr;

  x1 = new FieldElement(192, prime);
  y1 = new FieldElement(105, prime);
  x2 = new FieldElement(17, prime);
  y2 = new FieldElement(56, prime);
  FieldElementPoint p1 = FieldElementPoint(x1, y1, a, b);
  FieldElementPoint p2 = FieldElementPoint(x2, y2, a, b);
  cout << (p1 + p2).toString() << endl;
  delete x1;
  delete y1;
  delete x2;
  delete y2;

  x1 = new FieldElement(170, prime);
  y1 = new FieldElement(142, prime);
  x2 = new FieldElement(60, prime);
  y2 = new FieldElement(139, prime);
  cout << (FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b)).toString() << endl;
  delete x1;
  delete y1;
  delete x2;
  delete y2;

  x1 = new FieldElement(47, prime);
  y1 = new FieldElement(71, prime);
  x2 = new FieldElement(17, prime);
  y2 = new FieldElement(56, prime);
  cout << (FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b)).toString() << endl;
  delete x1;
  delete y1;
  delete x2;
  delete y2;

  x1 = new FieldElement(143, prime);
  y1 = new FieldElement(98, prime);
  x2 = new FieldElement(76, prime);
  y2 = new FieldElement(66, prime);
  cout << (FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b)).toString() << endl;
  delete x1;
  delete y1;
  delete x2;
  delete y2;

  delete a;
  delete b;
}

void testFieldElementPointScalarMultiplication() {
  cout << "testFieldElementPointScalarMultiplication()" << endl;
  int prime = 223;
  FieldElement* a = new FieldElement(0, prime);
  FieldElement* b = new FieldElement(7, prime);

  FieldElement* x1 = nullptr;
  FieldElement* y1 = nullptr;
  FieldElement* x2 = nullptr;
  FieldElement* y2 = nullptr;

  x1 = new FieldElement(192, prime);
  y1 = new FieldElement(105, prime);
  FieldElementPoint* p1 = new FieldElementPoint(x1, y1, a, b);
  cout << (*p1 + *p1).toString() << endl;
  delete x1;
  delete y1;
  delete p1;

  x1 = new FieldElement(143, prime);
  y1 = new FieldElement(98, prime);
  p1 = new FieldElementPoint(x1, y1, a, b);
  cout << (*p1 + *p1).toString() << endl;
  delete x1;
  delete y1;
  delete p1;

  x1 = new FieldElement(47, prime);
  y1 = new FieldElement(71, prime);
  p1 = new FieldElementPoint(x1, y1, a, b);
  cout << (*p1 + *p1).toString() << endl;
  cout << (*p1 + *p1 + *p1 + *p1).toString() << endl;
  cout << (*p1 + *p1 + *p1 + *p1 + *p1 + *p1 + *p1 + *p1).toString() << endl;
  delete x1;
  delete y1;
  delete p1;

  delete a;
  delete b;
}

int main() {
  FieldElementPoint* p0;
  defineFieldElementPoint(&p0);  
  cout << p0->toString() << endl;
  testIfPointsOnCurve();
  cout << endl;
  testFieldElementPointAddition();
  cout << endl;
  testFieldElementPointScalarMultiplication();
  return 0;
}