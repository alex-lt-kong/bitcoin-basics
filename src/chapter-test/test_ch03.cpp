#include <iostream>
#include <stdexcept>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/mod_inverse.hpp>

#include "mybitcoin/ecc.h"
#include "mybitcoin/utils.h"

using namespace std;

int testIfPointsOnCurve() {
    FieldElementPoint* fep = nullptr;

    try {
        fep = new FieldElementPoint(
            FieldElement(17, 223), FieldElement(56, 223), FieldElement(0, 223), FieldElement(7, 223)
        );        
    } catch (const invalid_argument& ia) {
        return 1;
    }
    if (fep == nullptr) {
        delete fep;    
        return 1;
    }
    delete fep;    
    fep = nullptr;

    try {
        fep = new FieldElementPoint(
            FieldElement(17, 223), FieldElement(56, 223), FieldElement(0, 223), FieldElement(7, 223)
        );        
    } catch (const invalid_argument& ia) {
        return 1;
    }
    if (fep == nullptr) {
        delete fep;    
        return 1;
    }
    delete fep;
    fep = nullptr;

    try {
        fep = new FieldElementPoint(
            FieldElement(1, 223), FieldElement(193, 223), FieldElement(0, 223), FieldElement(7, 223)
        );
    } catch (const invalid_argument& ia) {
        return 1;
    }
    if (fep == nullptr) {
        delete fep;    
        return 1;
    }
    delete fep;
    return 0;
}

int testIfPointsNotOnCurve() {
    FieldElementPoint* fep = nullptr;
    try {
        fep = new FieldElementPoint(
            FieldElement(200, 223), FieldElement(119, 223), FieldElement(0, 223), FieldElement(7, 223)
        );
        return 1;
    } catch (const invalid_argument& ia) {
        if (fep != nullptr) {
          return 1;
        }
    }
    
    try {
        fep = new FieldElementPoint(
            FieldElement(42, 223), FieldElement(99, 223), FieldElement(0, 223), FieldElement(7, 223)
        );
        return 1;
    } catch (const invalid_argument& ia) {
        if (fep != nullptr) {
          return 1;
        }
    }
    return 0;
}

int testFieldElementPointAddition() {
    int prime = 223;
    
    FieldElement a = FieldElement(0, prime);
    FieldElement b = FieldElement(7, prime);

    FieldElement x1;
    FieldElement y1;
    FieldElement x2;
    FieldElement y2;

    FieldElementPoint res = FieldElementPoint(a, b);

    x1 = FieldElement(192, prime);
    y1 = FieldElement(105, prime);
    x2 = FieldElement(17, prime);
    y2 = FieldElement(56, prime);
    FieldElementPoint p1 = FieldElementPoint(x1, y1, a, b);
    FieldElementPoint p2 = FieldElementPoint(x2, y2, a, b);
    res = p1 + p2;
    if (res.x().num() != 170) return 1;
    if (res.y().num() != 142) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    if (res.a().prime() != prime) return 1;
    if (res.b().prime() != prime) return 1;
    if (res.x().prime() != prime) return 1;
    if (res.y().prime() != prime) return 1;


    x1 = FieldElement(170, prime);
    y1 = FieldElement(142, prime);
    x2 = FieldElement(60, prime);
    y2 = FieldElement(139, prime);
    res = FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b);
    if (res.x().num() != 220) return 1;
    if (res.y().num() != 181) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    if (res.a().prime() != prime) return 1;
    if (res.b().prime() != prime) return 1;
    if (res.x().prime() != prime) return 1;
    if (res.y().prime() != prime) return 1;


    x1 = FieldElement(47, prime);
    y1 = FieldElement(71, prime);
    x2 = FieldElement(17, prime);
    y2 = FieldElement(56, prime);
    res = FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b);
    if (res.x().num() != 215) return 1;
    if (res.y().num() != 68) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    if (res.a().prime() != prime) return 1;
    if (res.b().prime() != prime) return 1;
    if (res.x().prime() != prime) return 1;
    if (res.y().prime() != prime) return 1;



    x1 = FieldElement(143, prime);
    y1 = FieldElement(98, prime);
    x2 = FieldElement(76, prime);
    y2 = FieldElement(66, prime);
    res = FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b);
    if (res.x().num() != 47) return 1;
    if (res.y().num() != 71) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    if (res.a().prime() != prime) return 1;
    if (res.b().prime() != prime) return 1;
    if (res.x().prime() != prime) return 1;
    if (res.y().prime() != prime) return 1;
    return 0;
}


int testFieldElementPointScalarMultiplication() {
    int prime = 223;
    int512_t num = -1;
    FieldElement a = FieldElement(0, prime);
    FieldElement b = FieldElement(7, prime);

    FieldElement x1;
    FieldElement y1;
    FieldElement x2;
    FieldElement y2;
    FieldElementPoint res = FieldElementPoint(a, b);
    x1 = FieldElement(192, prime);
    y1 = FieldElement(105, prime);
    FieldElementPoint p1 = FieldElementPoint(x1, y1, a, b);
    res = p1 + p1;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 49) return 1;
    if (res.y().num() != 71) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;

    res = p1 * 2;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 49) return 1;
    if (res.y().num() != 71) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    

    x1 = FieldElement(143, prime);
    y1 = FieldElement(98, prime);
    p1 = FieldElementPoint(x1, y1, a, b);
    res = p1 + p1;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 64) return 1;
    if (res.y().num() != 168) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;

    res = p1 * 2;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 64) return 1;
    if (res.y().num() != 168) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;

    x1 = FieldElement(47, prime);
    y1 = FieldElement(71, prime);
    p1 = FieldElementPoint(x1, y1, a, b);
    
    res = p1 + p1;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 36) return 1;
    if (res.y().num() != 111) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;

    res = p1 * 2;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 36) return 1;
    if (res.y().num() != 111) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;

    res = p1 * 4;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 194) return 1;
    if (res.y().num() != 51) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    res = p1 + p1 + p1 + p1;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 194) return 1;
    if (res.y().num() != 51) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;


    res = p1 * 8;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 116) return 1;
    if (res.y().num() != 55) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    res = p1 + p1 + p1 + p1 + p1 + p1 + p1 + p1;
    if (res.infinity() != false) return 1;
    if (res.x().num() != 116) return 1;
    if (res.y().num() != 55) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;

    res = p1 * 21;
    if (!res.infinity()) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    
    res = p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1;
    if (!res.infinity()) return 1;
    
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;

    
    try {
        num = res.y().num();
        return 1;
    } catch (const invalid_argument& ia) {
        if (num != -1) return 1;
    }
    x1 = FieldElement(15, prime);
    y1 = FieldElement(86, prime);
    p1 = FieldElementPoint(x1, y1, a, b);
    res = p1 * 7;
    if (!res.infinity()) return 1;
    num = -1;
    try {
        num = res.y().num();
        return 1;
    } catch (const invalid_argument& ia) {
        if (num != -1) return 1;
    }
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    res = res + p1;
    if (res.x().num() != 15) return 1;
    if (res.y().num() != 86) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    
    res = p1 * 8;
    if (res.x().num() != 15) return 1;
    if (res.y().num() != 86) return 1;
    if (res.a().num() != 0) return 1; 
    if (res.b().num() != 7) return 1;
    
    res = (p1 + p1 + p1 + p1) * 2;
    if (res.x().num() != 15) return 1;
    if (res.y().num() != 86) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    return 0;
}

int findOrderOfGroup() {
    int prime = 223;
    FieldElement a = FieldElement(0, prime);
    FieldElement b = FieldElement(7, prime);
    FieldElement x = FieldElement(15, prime);
    FieldElement y = FieldElement(86, prime);

    FieldElementPoint p = FieldElementPoint(a, b);
    FieldElementPoint orig = FieldElementPoint(x, y, a, b);
    int order = 0;
    while (order == 0 || p.infinity()    == false) {
        p += orig;
        order ++;
    }
    if (orig.x().num() != 15) return 1;
    if (orig.y().num() != 86) return 1;
    if (orig.a().num() != 0) return 1;
    if (orig.b().num() != 7) return 1;
    if (order != 7) return 1;
    return 0;
}

int testSecp256k1() {
    int512_t gx{"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"}; // string-based constructor
    int512_t gy{"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"};    
    // gx and gy are the coordinates of point g, which is the generator point, i.e., we add g to itself over and over again.
    int512_t prime = pow((int512_t)2, 256) - pow((int512_t)2,32) - 977;
    int512_t order{"0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"};
    // If we add g "order" number of times to itself, we get point at infinity;
    // If we add g "order + 1" number of times to itself, we get g again!
    FieldElementPoint g = FieldElementPoint(
        FieldElement(gx, prime), FieldElement(gy, prime), FieldElement(0, prime), FieldElement(7, prime)
    );
    FieldElementPoint res = g * order;
    if (!res.infinity()) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    if (res.a().prime() != (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f") return 1;
    if (!res.infinity()) return 1;
    int512_t num = -1;
    try {
        num = res.x().num();
        return 1;
    } catch (const invalid_argument& ia) {
        if (num != -1) return 1;
    }

    res = g * (order + 1);
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    if (res.x().num() != (int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798") return 1;
    if (res.y().num() != (int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8") return 1;
    if (res.a().prime() != (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f") return 1;
    return 0;
}

int testS256SubClass() {
    S256Element x = S256Element((int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
    S256Element y = S256Element((int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
    S256Element a = S256Element((int512_t)0);
    S256Element b = S256Element((int512_t)7);
    int512_t order{"0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"};
    FieldElementPoint g = FieldElementPoint(x, y, a, b);
    FieldElementPoint res = g * order;
    if (!res.infinity()) return 1;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    if (res.a().prime() != (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f") return 1;
    
    res = g * order + g;
    if (res.a().num() != 0) return 1;
    if (res.b().num() != 7) return 1;
    if (res.x().num() != (int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798") return 1;
    if (res.y().num() != (int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8") return 1;
    if (res.a().prime() != (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f") return 1;
    return 0;
}

int testS256Verification() {
    S256Point p1 = S256Point(
        (int512_t)"0x04519fac3d910ca7e7138f7013706f619fa8f033e6ec6e09370ea38cee6a7574",
        (int512_t)"0x82b51eab8c27c66e26c858a079bcdf4f1ada34cec420cafc7eac1a42216fb6c4"
    );
    Signature sig1 = Signature(
        (int512_t)"0x37206a0610995c58074999cb9767b87af4c4978db68c06e8e6e81d282047a7c6",
        (int512_t)"0x8ca63759c1157ebeaec0d03cecca119fc9a75bf8e6d0fa65c841c8e2738cdaec"
    );
    if (p1.verify((int512_t)"0xbc62d4b80d9e36da29c16c5d4d9f11731f36052c72401a76c23c0fb5a9b74423", sig1) != 1) return 1;
    if (p1.verify((int512_t)"0xbc62d4b80d9e36da29c16c5d4d9f11731f36052c72401a76c23c0fb5a9b74422", sig1) != 0) return 1;

    S256Point p2 = S256Point(
        (int512_t)"0x887387e452b8eacc4acfde10d9aaf7f6d9a0f975aabb10d006e4da568744d06c",
        (int512_t)"0x61de6d95231cd89026e286df3b6ae4a894a3378e393e93a0f45b666329a0ae34"
    );
    Signature sig2 = Signature(
        (int512_t)"0xac8d1c87e51d0d441be8b3dd5b05c8795b48875dffe00b7ffcfac23010d3a395",
        (int512_t)"0x68342ceff8935ededd102dd876ffd6ba72d6a427a3edb13d26eb0781cb423c4"
    );
    if (p2.verify((int512_t)"0xec208baa0fc1c19f708a9ca96fdeff3ac3f230bb4a7ba4aede4942ad003c0f60", sig2) != 1) return 1;
    if (p2.verify((int512_t)"0x7c076ff316692a3d7eb3c3bb0f8b1488cf72e1afcd929e29307032997a838a3d", sig2) != 0) return 1;

    Signature sig3 = Signature(
        (int512_t)"0xeff69ef2b1bd93a66ed5219add4fb51e11a840f404876325a1e8ffe0529a2c",
        (int512_t)"0xc7207fee197d27c618aea621406f6bf5ef6fca38681d82b2f06fddbdce6feab6"
    );
    if (p2.verify((int512_t)"0xec208baa0fc1c19f708a9ca96fdeff3ac3f230bb4a7ba4aede4942ad003c0f60", sig3) != 0) return 1;
    if (p2.verify((int512_t)"0x7c076ff316692a3d7eb3c3bb0f8b1488cf72e1afcd929e29307032997a838a3d", sig3) != 1) return 1;
    return 0;
}

int testBytesToInt512() {
    uint8_t input0[] = { 0xff, 0x00 };
    if (get_int512_from_bytes(input0, sizeof(input0), true) != 65280) return 1;
    if (get_int512_from_bytes(input0, sizeof(input0), false) != 255) return 1;

    uint8_t input1[] = { 0xde, 0xad, 0xbe, 0xef };
    if (get_int512_from_bytes(input1, sizeof(input1), true) != 3735928559) return 1;

    uint8_t input2[] = { 0x05, 0x43, 0x21, 0xde, 0xad, 0xbe, 0xef };
    if (get_int512_from_bytes(input2, sizeof(input2), false) != (int512_t)"67482173399188229") return 1;
    
    uint8_t input3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    if (get_int512_from_bytes(input3, sizeof(input3), false) != 0) return 1;

    uint8_t input4[] = { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xee };
    if (get_int512_from_bytes(input4, sizeof(input4), false) != (int512_t)"1123923222922975560859903") return 1;

    uint8_t input5[] = { 0x20, 0xB6, 0xEF, 0x40, 0x4A, 0xE5, 0x76 };
    if (get_int512_from_bytes(input5, sizeof(input5), false) != (int512_t)"33466154331649568") return 1; // 2018 ^ 5
    return 0;
}

int testSignatureCreation() {

    uint8_t secretChars[] = {'m', 'y', ' ', 's', 'e', 'c', 'r', 'e', 't' };
    uint8_t secretBytes[SHA256_HASH_SIZE];
    cal_sha256_hash(secretChars, sizeof(secretChars), secretBytes);
    cal_sha256_hash(secretBytes, SHA256_HASH_SIZE, secretBytes);
    int512_t secret = get_int512_from_bytes(secretBytes, SHA256_HASH_SIZE);

    uint8_t msgChars[] = {'m', 'y', ' ', 'm', 'e', 's', 's', 'a', 'g', 'e' };
    uint8_t msgHashBytes[SHA256_HASH_SIZE];
    cal_sha256_hash(msgChars, sizeof(msgChars), msgHashBytes);
    cal_sha256_hash(msgHashBytes, SHA256_HASH_SIZE, msgHashBytes);
    int512_t msgHash = get_int512_from_bytes(msgHashBytes, SHA256_HASH_SIZE);

    S256Point p = G * secret;
    int512_t k = 1234567890;
    int512_t r = (G * k).x().num();
    int512_t kInv = boost::integer::mod_inverse(k, G.order());
    int512_t sig = (int512_t)((int1024_t)(msgHash + secret * r) * kInv % G.order());
    if (strcmp(p.to_string().c_str(), "S256Point(028d003eab2e428d11983f3e97c3fa0addf3b42740df0d211795ffb3be2f6c52, 0ae987b9ec6ea159c78cb2a937ed89096fb218d9e7594f02b547526d8cd309e2)_0_7")) {
        return 1;
    }

    if (msgHash != (int512_t)"0x231c6f3d980a6b0fb7152f85cee7eb52bf92433d9919b9c5218cb08e79cce78") {
        return 1;
    }
    if (r != (int512_t)"0x2b698a0f0a4041b77e63488ad48c23e8e8838dd1fb7520408b121697b782ef22") {
        return 1;
    }
    if (sig != (int512_t)"0xbb14e602ef9e3f872e25fad328466b34e6734b7a0fcd58b1eb635447ffae8cb9") {
        return 1;
    }


    ECDSAKey pk = ECDSAKey(secretBytes, sizeof(secretBytes));
    if (pk.get_deterministic_k(msgHashBytes, sizeof(msgHashBytes)) != (int512_t)"0xfa4c6b87c29398c5022557788697f68dd3e2d39744355d18f47e4b91c69abad0") {
        return 1;
    }

    pk = ECDSAKey(12345);
    
    cal_sha256_hash((uint8_t*)"Programming Bitcoin!", strlen("Programming Bitcoin!"), msgHashBytes);
    cal_sha256_hash(msgHashBytes, SHA256_HASH_SIZE, msgHashBytes); 
    if (pk.get_deterministic_k(msgHashBytes, SHA256_HASH_SIZE) != (int512_t)"0xabef7a40d9bd76aef7ee7e733404ecfcd8041550a68625d7cc0608b0025038b1") {
        return 1;
    }
    if (strcmp(pk.sign(msgHashBytes, sizeof(msgHashBytes)).to_string().c_str(), "Signature(8eeacac05e4c29e793b5287ed044637132ce9ead7fded533e7441d87a8dc9c23, 36674f81f10c7fb347c1224bd546813ea24ada6f642c02f2248516e3aa8cb303)") != 0) {
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
        {"testIfPointsOnCurve()", &testIfPointsOnCurve},
        {"testIfPointsNotOnCurve()", &testIfPointsNotOnCurve},
        {"findOrderOfGroup()", &findOrderOfGroup},
        {"testFieldElementPointScalarMultiplication()", &testFieldElementPointScalarMultiplication},
        {"testSecp256k1()", &testSecp256k1},
        {"testS256SubClass()", &testS256SubClass},
        {"testS256Verification()", &testS256Verification},
        {"testBytesToInt512()", &testBytesToInt512},
        {"testSignatureCreation()", &testSignatureCreation},
        {"testFieldElementPointAddition()", &testFieldElementPointAddition}
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
