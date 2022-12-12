#include <sstream>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/ecc.h"
#include "../src/utils.h"

Test(ch04_test_suite, test_uncompressed_sec_format_from_bytes) {
  const size_t test_case_size = 2;
  uint8_t private_key_bytes[test_case_size][16] = {
    {0x00, 0x00, 0x13, 0x88}, // little-endian 5000 (0x1388)
    {0x0d, 0xea, 0xdb, 0xee, 0xf1, 0x23, 0x45} //0xdeadbeef12345   
  };
  size_t private_bytes_sizes[test_case_size] = {4, 7};
  char expected_sec_strs[test_case_size][65 * 2 + 1] = {
    "04ffe558e388852f0120e46af2d1b370f85854a8eb0841811ece0e3e03d282d57c315dc72890a4f10a1481c031b03b351b0dc79901ca18a00cf009dbdb157a1d10",
    "04d90cd625ee87dd38656dd95cf79f65f60f7273b67d3096e68bd81e4f5342691f842efa762fd59961d0e99803c61edba8b3e3f7dc3a341836f97733aebf987121"
  };
  for (size_t i = 0; i < test_case_size; ++i) {
    ECDSAKey pk = ECDSAKey(private_key_bytes[i], private_bytes_sizes[i]);
    uint8_t* sec = pk.public_key().get_sec_format(false);
    char* sec_str = bytes_to_hex_string(sec, 64 + 1, false);
    cr_expect(
      eq(str, sec_str, expected_sec_strs[i])
    ); 
    free(sec_str);
    free(sec);
  }
}

Test(ch04_test_suite, test_uncompressed_sec_format_from_int512) {
  const size_t test_case_size = 2;
  int512_t private_key_ints[test_case_size] = {
    5000,
    33466154331649568 // 2018 ^ 5
  };
  char expected_sec_strs[test_case_size][65 * 2 + 1] = {
    "04ffe558e388852f0120e46af2d1b370f85854a8eb0841811ece0e3e03d282d57c315dc72890a4f10a1481c031b03b351b0dc79901ca18a00cf009dbdb157a1d10",
    "04027f3da1918455e03c46f659266a1bb5204e959db7364d2f473bdf8f0a13cc9dff87647fd023c13b4a4994f17691895806e1b40b57f4fd22581a4f46851f3b06"
  };
  for (size_t i = 0; i < test_case_size; ++i) {
    ECDSAKey pk = ECDSAKey(private_key_ints[i]);
    uint8_t* sec = pk.public_key().get_sec_format(false);
    char* sec_str = bytes_to_hex_string(sec, 64 + 1, false);
    cr_expect(
      eq(str, sec_str, expected_sec_strs[i])
    ); 
    free(sec_str);
    free(sec);
  } 
}

Test(ch04_test_suite, test_compressed_sec_format_from_bytes) {
  const size_t test_case_size = 2;
  uint8_t private_key_bytes[test_case_size][16] = {
    {0x00, 0x00, 0x13, 0x89}, // little-endian 5001 (0x1389)
    {0x0d, 0xea, 0xdb, 0xee, 0xf5, 0x43, 0x21} //0xdeadbeef54321 
  };
  size_t private_bytes_sizes[test_case_size] = {4, 7};
  char expected_sec_strs[test_case_size][33 * 2 + 1] = {
    "0357a4f368868a8a6d572991e484e664810ff14c05c0fa023275251151fe0e53d1",
    "0296be5b1292f6c856b3c5654e886fc13511462059089cdf9c479623bfcbe77690"
  };
  for (size_t i = 0; i < test_case_size; ++i) {
    ECDSAKey pk3 = ECDSAKey(private_key_bytes[i], private_bytes_sizes[i]);
    uint8_t* sec = pk3.public_key().get_sec_format(true);
    char* sec_str = bytes_to_hex_string(sec, 32 + 1, false);
    cr_expect(
      eq(str, sec_str, expected_sec_strs[i])
    ); 
    free(sec_str);
    free(sec);
  }  
}

Test(ch04_test_suite, test_compressed_sec_format_from_int512) {
  const size_t test_case_size = 2;
  int512_t private_key_ints[test_case_size] = {
    5001,
    33549155665686099 // 2019 ^ 5
  };
  char expected_sec_strs[test_case_size][33 * 2 + 1] = {
    "0357a4f368868a8a6d572991e484e664810ff14c05c0fa023275251151fe0e53d1",
    "02933ec2d2b111b92737ec12f1c5d20f3233a0ad21cd8b36d0bca7a0cfa5cb8701"
  };
  for (size_t i = 0; i < test_case_size; ++i) {
    ECDSAKey pk = ECDSAKey(private_key_ints[i]);
    uint8_t* sec = pk.public_key().get_sec_format(true);
    char* sec_str = bytes_to_hex_string(sec, 32 + 1, false);
    cr_expect(
      eq(str, sec_str, expected_sec_strs[i])
    ); 
    free(sec_str);
    free(sec);
  } 
}

Test(ch04_test_suite, test_der_sig_format) {
  Signature sig = Signature(
    (int512_t)"0x37206a0610995c58074999cb9767b87af4c4978db68c06e8e6e81d282047a7c6",
    (int512_t)"0x8ca63759c1157ebeaec0d03cecca119fc9a75bf8e6d0fa65c841c8e2738cdaec"
  );
  size_t output_len;
  uint8_t* der = sig.get_der_format(&output_len);
  char* hex_str = bytes_to_hex_string(der, output_len, false);
  cr_expect(
    eq(
      str, hex_str,
      "3045022037206a0610995c58074999cb9767b87af4c4978db68c06e8e6e81d282047a7c60221008ca63759c1157ebeaec0d03cecca119fc9a75bf8e6d0fa65c841c8e2738cdaec"
    )
  );
  free(der);
  free(hex_str);
}

Test(ch04_test_suite, test_bytes_to_base58) {
  const size_t test_case_size = 6;
  uint8_t inputs[test_case_size][128] = {
    {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21}, // Hello World!
    "The quick brown fox jumps over the lazy dog.",
    {0x7c, 0x07, 0x6f, 0xf3, 0x16, 0x69, 0x2a, 0x3d, 0x7e, 0xb3, 0xc3, 0xbb, 0x0f, 0x8b, 0x14, 0x88, 0xcf, 0x72, 0xe1, 0xaf, 0xcd, 0x92, 0x9e, 0x29, 0x30, 0x70, 0x32, 0x99, 0x7a, 0x83, 0x8a, 0x3d},
    {0xef, 0xf6, 0x9e, 0xf2, 0xb1, 0xbd, 0x93, 0xa6, 0x6e, 0xd5, 0x21, 0x9a, 0xdd, 0x4f, 0xb5, 0x1e, 0x11, 0xa8, 0x40, 0xf4, 0x04, 0x87, 0x63, 0x25, 0xa1, 0xe8, 0xff, 0xe0, 0x52, 0x9a, 0x2c},
    {0xc7, 0x20, 0x7f, 0xee, 0x19, 0x7d, 0x27, 0xc6, 0x18, 0xae, 0xa6, 0x21, 0x40, 0x6f, 0x6b, 0xf5, 0xef, 0x6f, 0xca, 0x38, 0x68, 0x1d, 0x82, 0xb2, 0xf0, 0x6f, 0xdd, 0xbd, 0xce, 0x6f, 0xea, 0xb6},
    {0x00, 0x00, 0x28, 0x7f, 0xb4, 0xcd}
  };
  size_t inputs_size[test_case_size] = {12, 44, 32, 31, 32, 6};
  
  char expected_outputs[test_case_size][128] = {
    "2NEpo7TZRRrLZSi2U",
    "USm3fpXnKG5EUBx2ndxBDMPVciP5hGey2Jh4NDv6gmeo1LkMeiKrLJUUBk6Z",
    "9MA8fRQrT4u8Zj8ZRd6MAiiyaxb2Y1CMpvVkHQu5hVM6",
    "4fE3H2E6XMp4SsxtwinF7w9a34ooUrwWe4WsW1458Pd",
    "EQJsjkd6JaGwxrjEhfeqPenqHwrBmPQZjJGNSCHBkcF7",
    "11233QC4"
  };
  char* output = NULL;
  for (size_t i = 0; i < test_case_size; ++i) {
    output = encode_bytes_to_base58_string(inputs[i], inputs_size[i], true);
    cr_expect(eq(str, output, expected_outputs[i]));
    free(output);
  }
}

Test(ch04_test_suite, test_base58_checksum) {
  const size_t test_case_size = 3;
  uint8_t input_bytes[test_case_size][64] = {
    {0xde, 0xad, 0xbe, 0xef},
    "Hello world!",
    "The quick brown fox jumps over the lazy dog."
  };
  size_t input_bytes_len[test_case_size] = {4,12,44};
  char expected_outputs[test_case_size][128] = {
    "eFGDJPketnz",
    "9wWTEnNTWna86WmtFaRbXa",
    "46auvTd4NTVoJhFVnfh9reLsP21HQAQUFXCCBzNZjAPwQBRpaSp4aDLzWajGrqq21B"
  };  
  char* output;
  for (size_t i = 0; i < test_case_size; ++i) {
    output = encode_base58_checksum(input_bytes[i], input_bytes_len[i]);
    cr_expect(eq(str, output, expected_outputs[i]));
    free(output);
  }
}

void test_hash160_address() {
  cout << "test_hash160_address():" << endl;
  char* addr;

  ECDSAKey key = ECDSAKey(5002);
  addr = key.public_key().get_address(false, true);
  cout << "Function result: "<< addr << "\n"
       << "Expected result: mmTPbXQFxboEtNRkwfh6K51jvdtHLxGeMA" << endl;
  free(addr);

  ECDSAKey key2 = ECDSAKey((int512_t)2020 * 2020 * 2020 * 2020 * 2020);
  addr = key2.public_key().get_address(true, true);
  cout << "Function result: "<< addr << "\n"
       << "Expected result: mopVkxp8UhXqRYbCYJsbeE1h1fiF64jcoH" << endl;
  free(addr);

  uint8_t priv_key3[] = {0x01, 0x23, 0x45, 0xde, 0xad, 0xbe, 0xef};
  ECDSAKey key3= ECDSAKey(priv_key3, sizeof(priv_key3));
  addr = key3.public_key().get_address(true, false);
  cout << "Function result: "<< addr << "\n"
       << "Expected result: 1F1Pn2y6pDb68E5nYJJeba4TLg2U7B6KF1" << endl;
  free(addr);
}

void test_privkey_wif_address() {
  cout << "test_privkey_wif_address():" << endl;
  char* addr;

  ECDSAKey key = ECDSAKey(5003);
  addr = key.get_wif_private_key(true, true);
  cout << "Function result: "<< addr << "\n"
       << "Expected result: cMahea7zqjxrtgAbB7LSGbcQUr1uX1ojuat9jZodMN8rFTv2sfUK" << endl;
  free(addr);

  ECDSAKey key2 = ECDSAKey((int512_t)2021 * 2021 * 2021 * 2021 * 2021);
  addr = key2.get_wif_private_key(false, true);
  cout << "Function result: "<< addr << "\n"
       << "Expected result: 91avARGdfge8E4tZfYLoxeJ5sGBdNJQH4kvjpWAxgzczjbCwxic" << endl;
  free(addr);

  uint8_t priv_key3[] = {0x05, 0x43, 0x21, 0xde, 0xad, 0xbe, 0xef};
  ECDSAKey key3 = ECDSAKey(priv_key3, sizeof(priv_key3));
  addr = key3.get_wif_private_key(true, false);
  cout << "Function result: "<< addr << "\n"
       << "Expected result: KwDiBf89QgGbjEhKnhXJuH7LrciVrZi3qYjgiuQJv1h8Ytr2S53a" << endl;
  free(addr);
}
/*
int main() {
  test_uncompressed_sec_format();
  cout << endl;
  test_compressed_sec_format();
  cout << endl;
  test_der_sig_format();
  cout << endl;
  test_base58();
  cout << endl;
  test_base58_checksum();
  cout << endl;
  test_hash160_address();
  cout << endl;
  test_privkey_wif_address();
  cout << endl;
}*/
