#include <float.h>
#include <iostream>
#include <stdexcept>
#include <limits.h>
#include <math.h>

#include "field-element-point.h"

using namespace std;

int main() {

  FieldElementPoint p1 = FieldElementPoint(
    FieldElement(192, 223), FieldElement(105, 223), FieldElement(0, 223), FieldElement(7, 223)
  );
  cout << p1.toString() << endl;
  return 0;
}