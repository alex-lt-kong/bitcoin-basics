#include <iostream>
#include <stdexcept>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/mod_inverse.hpp>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/ecc.h"
#include "../src/utils.h"

using namespace std;

Test(ch03_test_suite, testIfPointsOnCurve) {
  FieldElementPoint* fep = nullptr;

  try {
    fep = new FieldElementPoint(
      FieldElement(17, 223), FieldElement(56, 223), FieldElement(0, 223), FieldElement(7, 223)
    );    
  } catch (const invalid_argument& ia) {
    cr_fail("No exception should be thrown.");
  }
  cr_assert(fep != nullptr);
  delete fep;  
  fep = nullptr;

  try {
    fep = new FieldElementPoint(
      FieldElement(17, 223), FieldElement(56, 223), FieldElement(0, 223), FieldElement(7, 223)
    );    
  } catch (const invalid_argument& ia) {
    cr_fail("No exception should be thrown.");
  }
  cr_assert(fep != nullptr);
  delete fep;
  fep = nullptr;

  try {
    fep = new FieldElementPoint(
      FieldElement(1, 223), FieldElement(193, 223), FieldElement(0, 223), FieldElement(7, 223)
    );
  } catch (const invalid_argument& ia) {
    cr_fail("No exception should be thrown.");
  }
  cr_assert(fep != nullptr);
  delete fep;
}

Test(ch03_test_suite, testIfPointsNotOnCurve) {
  FieldElementPoint* fep = nullptr;
  try {
    fep = new FieldElementPoint(
      FieldElement(200, 223), FieldElement(119, 223), FieldElement(0, 223), FieldElement(7, 223)
    );
    cr_fail("Exception should be thrown.");
  } catch (const invalid_argument& ia) {
    cr_assert(fep == nullptr);
  }
  
  try {
    fep = new FieldElementPoint(
      FieldElement(42, 223), FieldElement(99, 223), FieldElement(0, 223), FieldElement(7, 223)
    );
    cr_fail("Exception should be thrown.");
  } catch (const invalid_argument& ia) {
    cr_assert(fep == nullptr);
  }
}

Test(ch03_test_suite, testFieldElementPointAddition) {
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
  cr_expect(res.x().num() == 170);
  cr_expect(res.y().num() == 142);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  cr_expect(res.a().prime() == prime);
  cr_expect(res.b().prime() == prime);
  cr_expect(res.x().prime() == prime);
  cr_expect(res.y().prime() == prime);


  x1 = FieldElement(170, prime);
  y1 = FieldElement(142, prime);
  x2 = FieldElement(60, prime);
  y2 = FieldElement(139, prime);
  res = FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b);
  cr_expect(res.x().num() == 220);
  cr_expect(res.y().num() == 181);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  cr_expect(res.a().prime() == prime);
  cr_expect(res.b().prime() == prime);
  cr_expect(res.x().prime() == prime);
  cr_expect(res.y().prime() == prime);


  x1 = FieldElement(47, prime);
  y1 = FieldElement(71, prime);
  x2 = FieldElement(17, prime);
  y2 = FieldElement(56, prime);
  res = FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b);
  cr_expect(res.x().num() == 215);
  cr_expect(res.y().num() == 68);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  cr_expect(res.a().prime() == prime);
  cr_expect(res.b().prime() == prime);
  cr_expect(res.x().prime() == prime);
  cr_expect(res.y().prime() == prime);



  x1 = FieldElement(143, prime);
  y1 = FieldElement(98, prime);
  x2 = FieldElement(76, prime);
  y2 = FieldElement(66, prime);
  res = FieldElementPoint(x1, y1, a, b) + FieldElementPoint(x2, y2, a, b);
  cr_expect(res.x().num() == 47);
  cr_expect(res.y().num() == 71);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  cr_expect(res.a().prime() == prime);
  cr_expect(res.b().prime() == prime);
  cr_expect(res.x().prime() == prime);
  cr_expect(res.y().prime() == prime);

}

Test(ch03_test_suite, testFieldElementPointScalarMultiplication) {
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
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 49);
  cr_expect(res.y().num() == 71);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);

  res = p1 * 2;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 49);
  cr_expect(res.y().num() == 71);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  

  x1 = FieldElement(143, prime);
  y1 = FieldElement(98, prime);
  p1 = FieldElementPoint(x1, y1, a, b);
  res = p1 + p1;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 64);
  cr_expect(res.y().num() == 168);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);

  res = p1 * 2;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 64);
  cr_expect(res.y().num() == 168);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);

  x1 = FieldElement(47, prime);
  y1 = FieldElement(71, prime);
  p1 = FieldElementPoint(x1, y1, a, b);
  
  res = p1 + p1;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 36);
  cr_expect(res.y().num() == 111);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);

  res = p1 * 2;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 36);
  cr_expect(res.y().num() == 111);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);

  res = p1 * 4;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 194);
  cr_expect(res.y().num() == 51);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  res = p1 + p1 + p1 + p1;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 194);
  cr_expect(res.y().num() == 51);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);


  res = p1 * 8;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 116);
  cr_expect(res.y().num() == 55);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  res = p1 + p1 + p1 + p1 + p1 + p1 + p1 + p1;
  cr_expect(res.infinity() == false);
  cr_expect(res.x().num() == 116);
  cr_expect(res.y().num() == 55);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);

  res = p1 * 21;
  cr_expect(res.infinity());
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  
  res = p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1+p1;
  cr_expect(res.infinity());
  
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);

  
  try {
    num = res.y().num();
    cr_fail("Exception should be thrown.");
  } catch (const invalid_argument& ia) {
    cr_expect(num == -1);
  }
  x1 = FieldElement(15, prime);
  y1 = FieldElement(86, prime);
  p1 = FieldElementPoint(x1, y1, a, b);
  res = p1 * 7;
  cr_expect(res.infinity());
  num = -1;
  try {
    num = res.y().num();
    cr_fail("Exception should be thrown.");
  } catch (const invalid_argument& ia) {
    cr_expect(num == -1);
  }
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  res = res + p1;
  cr_expect(res.x().num() == 15);
  cr_expect(res.y().num() == 86);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  
  res = p1 * 8;
  cr_expect(res.x().num() == 15);
  cr_expect(res.y().num() == 86);
  cr_expect(res.a().num() == 0);  
  cr_expect(res.b().num() == 7);
  
  res = (p1 + p1 + p1 + p1) * 2;
  cr_expect(res.x().num() == 15);
  cr_expect(res.y().num() == 86);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
}

Test(ch03_test_suite, findOrderOfGroup) {
  int prime = 223;
  FieldElement a = FieldElement(0, prime);
  FieldElement b = FieldElement(7, prime);
  FieldElement x = FieldElement(15, prime);
  FieldElement y = FieldElement(86, prime);

  FieldElementPoint p = FieldElementPoint(a, b);
  FieldElementPoint orig = FieldElementPoint(x, y, a, b);
  int order = 0;
  while (order == 0 || p.infinity()  == false) {
    p += orig;
    order ++;
  }
  cr_expect(orig.x().num() == 15);
  cr_expect(orig.y().num() == 86);
  cr_expect(orig.a().num() == 0);
  cr_expect(orig.b().num() == 7);
  cr_expect(eq(int, order, 7));
}

Test(ch03_test_suite, testSecp256k1) {
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
  cr_expect(res.infinity());
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  cr_expect(res.a().prime() == (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  cr_expect(res.infinity());
  int512_t num = -1;
  try {
    num = res.x().num();
    cr_fail("Exception should be thrown.");
  } catch (const invalid_argument& ia) {
    cr_expect(num == -1);
  }

  res = g * (order + 1);
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  cr_expect(res.x().num() == (int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
  cr_expect(res.y().num() == (int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
  cr_expect(res.a().prime() == (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
}

Test(ch03_test_suite, testS256SubClass) {
  S256Element x = S256Element((int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
  S256Element y = S256Element((int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
  S256Element a = S256Element((int512_t)0);
  S256Element b = S256Element((int512_t)7);
  int512_t order{"0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"};
  FieldElementPoint g = FieldElementPoint(x, y, a, b);
  FieldElementPoint res = g * order;
  cr_expect(res.infinity());  
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  cr_expect(res.a().prime() == (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  
  res = g * order + g;
  cr_expect(res.a().num() == 0);
  cr_expect(res.b().num() == 7);
  cr_expect(res.x().num() == (int512_t)"0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
  cr_expect(res.y().num() == (int512_t)"0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
  cr_expect(res.a().prime() == (int512_t)"0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
}

Test(ch03_test_suite, testS256Verification) {
  S256Point p1 = S256Point(
    (int512_t)"0x04519fac3d910ca7e7138f7013706f619fa8f033e6ec6e09370ea38cee6a7574",
    (int512_t)"0x82b51eab8c27c66e26c858a079bcdf4f1ada34cec420cafc7eac1a42216fb6c4"
  );
  Signature sig1 = Signature(
    (int512_t)"0x37206a0610995c58074999cb9767b87af4c4978db68c06e8e6e81d282047a7c6",
    (int512_t)"0x8ca63759c1157ebeaec0d03cecca119fc9a75bf8e6d0fa65c841c8e2738cdaec"
  );
  cr_expect(eq(i32, p1.verify((int512_t)"0xbc62d4b80d9e36da29c16c5d4d9f11731f36052c72401a76c23c0fb5a9b74423", sig1), 1));
  cr_expect(eq(i32, p1.verify((int512_t)"0xbc62d4b80d9e36da29c16c5d4d9f11731f36052c72401a76c23c0fb5a9b74422", sig1), 0));

  S256Point p2 = S256Point(
    (int512_t)"0x887387e452b8eacc4acfde10d9aaf7f6d9a0f975aabb10d006e4da568744d06c",
    (int512_t)"0x61de6d95231cd89026e286df3b6ae4a894a3378e393e93a0f45b666329a0ae34"
  );
  Signature sig2 = Signature(
    (int512_t)"0xac8d1c87e51d0d441be8b3dd5b05c8795b48875dffe00b7ffcfac23010d3a395",
    (int512_t)"0x68342ceff8935ededd102dd876ffd6ba72d6a427a3edb13d26eb0781cb423c4"
  );
  cr_expect(eq(i32, p2.verify((int512_t)"0xec208baa0fc1c19f708a9ca96fdeff3ac3f230bb4a7ba4aede4942ad003c0f60", sig2), 1));
  cr_expect(eq(i32, p2.verify((int512_t)"0x7c076ff316692a3d7eb3c3bb0f8b1488cf72e1afcd929e29307032997a838a3d", sig2), 0));

  Signature sig3 = Signature(
    (int512_t)"0xeff69ef2b1bd93a66ed5219add4fb51e11a840f404876325a1e8ffe0529a2c",
    (int512_t)"0xc7207fee197d27c618aea621406f6bf5ef6fca38681d82b2f06fddbdce6feab6"
  );
  cr_expect(eq(i32, p2.verify((int512_t)"0xec208baa0fc1c19f708a9ca96fdeff3ac3f230bb4a7ba4aede4942ad003c0f60", sig3), 0));
  cr_expect(eq(i32, p2.verify((int512_t)"0x7c076ff316692a3d7eb3c3bb0f8b1488cf72e1afcd929e29307032997a838a3d", sig3), 1));
}

Test(ch03_test_suite, testBytesToInt512) {
  uint8_t input0[] = { 0xff, 0x00 };
  cr_expect(get_int512_from_bytes(input0, sizeof(input0), true) == 65280);
  cr_expect(get_int512_from_bytes(input0, sizeof(input0), false) == 255);

  uint8_t input1[] = { 0xde, 0xad, 0xbe, 0xef };
  cr_expect(get_int512_from_bytes(input1, sizeof(input1), true) == 3735928559);

  uint8_t input2[] = { 0x05, 0x43, 0x21, 0xde, 0xad, 0xbe, 0xef };
  cr_expect(get_int512_from_bytes(input2, sizeof(input2), false) == (int512_t)"67482173399188229");
  
  uint8_t input3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  cr_expect(get_int512_from_bytes(input3, sizeof(input3), false) == 0);

  uint8_t input4[] = { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xee };
  cr_expect(get_int512_from_bytes(input4, sizeof(input4), false) == (int512_t)"1123923222922975560859903");

  uint8_t input5[] = { 0x20, 0xB6, 0xEF, 0x40, 0x4A, 0xE5, 0x76 };
  cr_expect(get_int512_from_bytes(input5, sizeof(input5), false) == (int512_t)"33466154331649568"); // 2018 ^ 5
}

void testSignatureCreation() {
  cout << "testSignatureCreation():" << endl;

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
  cout << "Result: p: " << p.to_string() << "\n"
       << "Expect: p: S256Point(028d003eab2e428d11983f3e97c3fa0addf3b42740df0d211795ffb3be2f6c52, 0ae987b9ec6ea159c78cb2a937ed89096fb218d9e7594f02b547526d8cd309e2)_0_7"
       << endl;
  cout << "Result: msgHash: " << hex << msgHash << dec << "\n"
       << "Expect: msgHash: 231c6f3d980a6b0fb7152f85cee7eb52bf92433d9919b9c5218cb08e79cce78"
       << endl;
  cout << "Result: r: " << hex << r << dec << "\n"
       << "Expect: r: 2b698a0f0a4041b77e63488ad48c23e8e8838dd1fb7520408b121697b782ef22"
       << endl;
  cout << "Result: sig: " << hex << sig << dec << "\n"
       << "Expect: sig: bb14e602ef9e3f872e25fad328466b34e6734b7a0fcd58b1eb635447ffae8cb9"
       << endl;


  ECDSAKey pk = ECDSAKey(secretBytes, sizeof(secretBytes));
  cout << "Result: " << hex << pk.get_deterministic_k(msgHashBytes, sizeof(msgHashBytes)) << dec << endl;
  cout << "Expect: fa4c6b87c29398c5022557788697f68dd3e2d39744355d18f47e4b91c69abad0" << endl;

  pk = ECDSAKey(12345);
  
  cal_sha256_hash((uint8_t*)"Programming Bitcoin!", strlen("Programming Bitcoin!"), msgHashBytes);
  cal_sha256_hash(msgHashBytes, SHA256_HASH_SIZE, msgHashBytes);  
  cout << "Result: Deterministic K: " << hex << pk.get_deterministic_k(msgHashBytes, SHA256_HASH_SIZE) << dec << "\n"
       << "Expect: Deterministic K: abef7a40d9bd76aef7ee7e733404ecfcd8041550a68625d7cc0608b0025038b1"
       << endl;

  cout << "Result: " << pk.sign(msgHashBytes, sizeof(msgHashBytes)).to_string() << "\n"
       << "Expect: Signature(8eeacac05e4c29e793b5287ed044637132ce9ead7fded533e7441d87a8dc9c23, 36674f81f10c7fb347c1224bd546813ea24ada6f642c02f2248516e3aa8cb303)"
       << endl;
}
