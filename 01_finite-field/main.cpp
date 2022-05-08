#include <iostream>
#include <stdexcept>
#include <math.h>
#include "../ecc.h"

using namespace std;

void testDivision() {
  FieldElement *a = new FieldElement(7, 19);
  FieldElement b = FieldElement(5, 19);
  cout << "Test division:\n"
       << (FieldElement(2, 19)/FieldElement(7, 19)).toString() << "\n"
       << ((*a)/b).toString() << endl;
  delete a;
}

void testPower() {
  FieldElement *a = new FieldElement(3, 13);
  FieldElement b = FieldElement(1, 13);
  cout << "Test power:\n"
      << ((*a).power(3) == b) << "\n"
      << (FieldElement(95, 97) * FieldElement(45, 97) * FieldElement(31, 97)).toString()
      << endl;
  delete a;
}

void testAddition() {
  FieldElement *a = new FieldElement(7,  13);
  FieldElement b = FieldElement(12, 13);
  FieldElement c = FieldElement(6,  13);
  // We can define an object in two ways.
  // The 1st way, heap memory is used, we need to take care of it ourselves
  // The 2nd way, stack memory is used, the object will be destoryed automatically. 
  cout << "Test addition:\n" << (*a + b == c) << endl;
  cout << (c * (*a)).toString() << endl;

  delete a;
}

int main() {
  testAddition();
  testPower();
  testDivision();
  cout << "Exercise 8" << endl;
  cout << (FieldElement(3, 31)/FieldElement(24, 31)).toString() << endl;
  cout << (FieldElement(17, 31).power(-3)).toString() << endl;
  cout << (FieldElement(4, 31).power(-4)*FieldElement(11, 31)).toString() << endl;
  return 0;
}
