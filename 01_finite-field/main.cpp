#include <iostream>
#include <stdexcept>
#include <math.h>
#include "../ecc.h"
#include "../utils.h"

using namespace std;

void testAddition() {
  cout << "testAddition():" << endl;
  FieldElement *a = new FieldElement(7,  13);
  FieldElement b = FieldElement(12, 13);
  FieldElement c = FieldElement(6,  13);
  // We can define an object in two ways.
  // The 1st way, heap memory is used, we need to take care of it ourselves
  // The 2nd way, stack memory is used, the object will be destoryed automatically. 
  cout << "Result: " << (*a + b == c) << "\n"
       << "Expect: " << "1\n" << endl;
  delete a;
}

void testDivision() {
  FieldElement *a = new FieldElement(7, 19);
  FieldElement b = FieldElement(5, 19);
  cout << "Test division:\n"
       << "Result: " << (FieldElement(2, 19)/FieldElement(7, 19)).to_string() << "\n"
       << "Expect: 3 (19)" << "\n"
       << "Result: " << ((*a)/b).to_string() << "\n"
       << "Expect: 9 (19)" << endl;
  delete a;
}

void testPower() {
  cout << "testPower():" << endl;
  FieldElement *a = new FieldElement(3, 13);
  FieldElement b = FieldElement(1, 13);
  cout << "Result: " << ((*a).power(3) == b) << "\n"
       << "Expect: 1\n"
       << "Result: " << (FieldElement(12, 97).power(7) * FieldElement(77, 97).power(49)).to_string()
       << "Expect: 63 (97)"
       << endl;
  delete a;
}

void testMultiplication() {
  cout << "testMultiplication():" << endl;
  FieldElement a = FieldElement(3, 13);
  FieldElement b = FieldElement(12, 13);
  FieldElement c = FieldElement(10, 13);
  cout << "Result: " << (a * b == c) << "\n"
       << "Expect: 1" << endl;
  cout << "Result: " << (a * b).to_string() << "\n"
       << "Expect: 10 (13)" << endl;
  cout << "Result: " << (FieldElement(95, 97) * FieldElement(45, 97) * FieldElement(31, 97)).to_string() << "\n"
       << "Expect: 23 (97)" << endl;
  cout << "Result: " << (FieldElement(17, 97) * FieldElement(13, 97) * FieldElement(19, 97) * FieldElement(44, 97)).to_string() << "\n"
       << "Expect: 68 (97)" << endl;
}

void exercise8() {
  cout << "exercise8():" << endl;
  cout << "Result: " << (FieldElement(3, 31)/FieldElement(24, 31)).to_string() << "\n"
       << "Expect: 4 (31)" << endl;
  cout << "Result: " << (FieldElement(17, 31).power(-3)).to_string() << "\n"
       << "Expect: 29 (31)" << endl;
  cout << "Result: " << (FieldElement(4, 31).power(-4)*FieldElement(11, 31)).to_string() << "\n"
       << "Expect: 13 (31)" << endl;
}

void test_fermat_primality_test() {
  cout << "test_fermat_primality_test():" << endl;
  for (int i = 1; i < 1024; i++) {
    if (fermat_primality_test(i, 1024) == true) {
      cout << i << ", ";
    }
  }  
  cout << endl;
}

int main() {
  testAddition();
  cout << endl;
  testMultiplication();
  cout << endl;
  testPower();
  cout << endl;
  testDivision();
  cout << endl;
  exercise8();
  cout << endl;
  test_fermat_primality_test();
  return 0;
}
