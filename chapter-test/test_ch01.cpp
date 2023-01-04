#include <iostream>
#include <stdexcept>
#include <math.h>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "mybitcoin/ecc.h"
#include "mybitcoin/utils.h"

using namespace std;

Test(ch01_test_suite, testAddition) {
  FieldElement a = FieldElement(7,  13);
  FieldElement b = FieldElement(12, 13);
  FieldElement c = FieldElement(6,  13);
  cr_expect(a + b == c);
}

Test(ch01_test_suite, testDivision) {

  FieldElement quotient = FieldElement(2, 19) / FieldElement(7, 19);
  cr_expect(quotient.num() == 3);
  cr_expect(quotient.prime() == 19);

  FieldElement a = FieldElement(7, 19);
  FieldElement b = FieldElement(5, 19);
  quotient = a / b;
  cr_expect(quotient.num() == 9);
  cr_expect(quotient.prime() == 19);
}

Test(ch01_test_suite, testPower) {
  FieldElement a = FieldElement(3, 13);
  FieldElement b = FieldElement(1, 13);
  FieldElement res = a.power(3);
  cr_expect(res == b);

  res = FieldElement(12, 97).power(7) * FieldElement(77, 97).power(49);
  cr_expect(res.num() == 63);
  cr_expect(res.prime() == 97);
}

Test(ch01_test_suite, testMultiplication) {
  FieldElement a = FieldElement(3, 13);
  FieldElement b = FieldElement(12, 13);
  FieldElement c = FieldElement(10, 13);
  FieldElement res = a * b;
  cr_expect(res == c);
  cr_expect(res.num() == 10);
  cr_expect(res.prime() == 13);
  
  res = FieldElement(95, 97) * FieldElement(45, 97) * FieldElement(31, 97);
  cr_expect(res.num() == 23);
  cr_expect(res.prime() == 97);
  
  res = FieldElement(17, 97) * FieldElement(13, 97) * FieldElement(19, 97) * FieldElement(44, 97);
  cr_expect(res.num() == 68);
  cr_expect(res.prime() == 97);
}


Test(ch01_test_suite, exercise8) {
  FieldElement a = FieldElement(3, 13);
  FieldElement b = FieldElement(12, 13);
  FieldElement c = FieldElement(10, 13);
  FieldElement res = a * b;
  cr_expect(res == c);
  cr_expect(res.num() == 10);
  cr_expect(res.prime() == 13);
  
  res = FieldElement(95, 97) * FieldElement(45, 97) * FieldElement(31, 97);
  cr_expect(res.num() == 23);
  cr_expect(res.prime() == 97);
  
  res = FieldElement(17, 97) * FieldElement(13, 97) * FieldElement(19, 97) * FieldElement(44, 97);
  cr_expect(res.num() == 68);
  cr_expect(res.prime() == 97);
}

void exercise8() {
  FieldElement res = FieldElement(3, 31)/FieldElement(24, 31);
  cr_expect(res.num() == 4);
  cr_expect(res.prime() == 31);

  res = FieldElement(17, 31).power(-3);
  cr_expect(res.num() == 29);
  cr_expect(res.prime() == 31);

  res = FieldElement(4, 31).power(-4)*FieldElement(11, 31);
  cr_expect(res.num() == 13);
  cr_expect(res.prime() == 31);
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
/*
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
*/