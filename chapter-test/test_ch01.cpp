#include <iostream>
#include <stdexcept>
#include <math.h>

#include "mybitcoin/ecc.h"
#include "mybitcoin/utils.h"

using namespace std;

int testAddition() {
    FieldElement a = FieldElement(7, 13);
    FieldElement b = FieldElement(12, 13);
    FieldElement c = FieldElement(6, 13);
    return a + b != c;
}

int testDivision() {

    FieldElement quotient = FieldElement(2, 19) / FieldElement(7, 19);
    if (quotient.num() != 3) {
        return 1;
    }
    if (quotient.prime() != 19) {
        return 1;
    }

    FieldElement a = FieldElement(7, 19);
    FieldElement b = FieldElement(5, 19);
    quotient = a / b;
    if (quotient.num() != 9) {
        return 1;
    }
    if (quotient.prime() != 19) {
        return 1;
    }
    return 0;
}

int testPower() {
    FieldElement a = FieldElement(3, 13);
    FieldElement b = FieldElement(1, 13);
    FieldElement res = a.power(3);
    if (res != b) {
        return 1;
    }

    res = FieldElement(12, 97).power(7) * FieldElement(77, 97).power(49);
    if (res.num() != 63) {
        return 1;
    }
    if (res.prime() != 97) {
        return 1;
    }
    return 0;
}

int testMultiplication() {
    FieldElement a = FieldElement(3, 13);
    FieldElement b = FieldElement(12, 13);
    FieldElement c = FieldElement(10, 13);
    FieldElement res = a * b;
    if (res != c || res.num() != 10 || res.prime() != 13) {
        return 1;
    }
    
    res = FieldElement(95, 97) * FieldElement(45, 97) * FieldElement(31, 97);
    if (res.num() != 23) {
        return 1;
    }
    if (res.prime() != 97) {
        return 1;
    }
    
    res = FieldElement(17, 97) * FieldElement(13, 97) * FieldElement(19, 97) * FieldElement(44, 97);
    if (res.num() != 68) {
        return 1;
    }
    if (res.prime() != 97) {
        return 1;
    }
    return 0;
}


int exercise8() {
    FieldElement a = FieldElement(3, 13);
    FieldElement b = FieldElement(12, 13);
    FieldElement c = FieldElement(10, 13);
    FieldElement res = a * b;
    if (res != c || res.num() != 10 || res.prime() != 13) {
        return 1;
    }
    
    res = FieldElement(95, 97) * FieldElement(45, 97) * FieldElement(31, 97);
    if (res.num() != 23 || res.prime() != 97) {
        return 1;
    }
    
    res = FieldElement(17, 97) * FieldElement(13, 97) * FieldElement(19, 97) * FieldElement(44, 97);
    if (res.num() != 68 || res.prime() != 97) {
        return 1;
    }
    return 0;
}

int exercise9() {
    FieldElement res = FieldElement(3, 31) / FieldElement(24, 31);
    if (res.num() != 4 || res.prime() != 31) {
        return 1;
    }

    res = FieldElement(17, 31).power(-3);
    if (res.num() != 29 || res.prime() != 31) {
        return 1;
    }

    res = FieldElement(4, 31).power(-4)*FieldElement(11, 31);
    if (res.num() != 13 || res.prime() != 31) {
        return 1;
    }
    return 0;
}

int main() {
    int retval = 0;
    struct Test_Suite {
        char test_name[128];
        int (*test_func)(void);
    };
     
    struct Test_Suite test_suites[] = {
        {"testAddition()", &testAddition},
        {"testDivision()", &testDivision},
        {"testPower()", &testPower},
        {"testMultiplication()", &testMultiplication},
        {"exercise8()", &exercise8},
        {"exercise9()", &exercise9},
    };

    for (uint32_t i = 0; i < sizeof(test_suites)/sizeof(test_suites[0]); ++i) {
        printf("testing %s...\n", test_suites[i].test_name);
        if (test_suites[i].test_func() != 0) {
            ++retval;
            fprintf(stderr, "FAILED!!!\n");
        }
    }

    if (retval != 0) {
        fprintf(stderr, "===== %d TEST(s) FAILED!!! =====\n", retval);
    } else {
        printf("All tests passed\n");
    }
    return retval;
}
