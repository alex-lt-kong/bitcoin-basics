#include <float.h>
#include <iostream>
#include <stdexcept>
#include <limits.h>
#include <math.h>

#include "field-element-point.h"

using namespace std;

void test0(FieldElementPoint **p) {
  // A fancy way to initialize a pointer by reference
  *p = new FieldElementPoint(
    new FieldElement(192, 223), new FieldElement(105, 223), new FieldElement(0, 223), new FieldElement(7, 223)
  );
}

int main() {
  FieldElementPoint* p0;
  test0(&p0);
  cout << p0->toString() << endl;
  return 0;
}