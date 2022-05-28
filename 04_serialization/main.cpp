#include "ecc.h"
#include "utils.h"
#include "misc.h"
#include "sha256.h"
#include "hmac.h"

void test_uncompressed_sec_format() {
  cout << "test_uncompressed_sec_format():" << endl;
  unsigned char private_key_bytes3[] = { 0x0d, 0xea, 0xdb, 0xee, 0xf1, 0x23, 0x45 };
  ECDSAKey pk3 = ECDSAKey(private_key_bytes3, sizeof(private_key_bytes3));
  unsigned char* sec = pk3.public_key().get_sec_format(false);
  for (int i = 0; i < (32 * 2 + 1); ++i)
    cout << hex << setfill('0') << setw(2) << (int)sec[i] << "";
  cout << endl;
  delete sec;
}

void test_compressed_sec_format() {
  cout << "test_compressed_sec_format():" << endl;
  unsigned char private_key_bytes3[] = { 0x0d, 0xea, 0xdb, 0xee, 0xf5, 0x43, 0x21 };
  ECDSAKey pk3 = ECDSAKey(private_key_bytes3, sizeof(private_key_bytes3));
  unsigned char* sec = pk3.public_key().get_sec_format(true);
  for (int i = 0; i < (32 + 1); ++i)
    cout << hex << setfill('0') << setw(2) << (int)sec[i] << "";
  cout << endl;
  delete sec;
}


void test_der_sig_format() {
  cout << "test_der_sig_format():" << endl;
  Signature sig = Signature(
    (int512_t)"0x37206a0610995c58074999cb9767b87af4c4978db68c06e8e6e81d282047a7c6",
    (int512_t)"0x8ca63759c1157ebeaec0d03cecca119fc9a75bf8e6d0fa65c841c8e2738cdaec"
  );
  sig.get_der_format();
  cout << "Corrent answer should be:\n"
       << "3045022037206a0610995c58074999cb9767b87af4c4978db68c06e8e6e81d282047a7c6022100"
       << "8ca63759c1157ebeaec0d03cecca119fc9a75bf8e6d0fa65c841c8e2738cdaec" << endl;
}

int main() {
  test_uncompressed_sec_format();
  cout << endl;
  test_compressed_sec_format();
  cout << endl;
  test_der_sig_format();
  cout << endl;
}