#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "tx.h"
#include "utils.h"
#include "./cryptographic-algorithms/src/misc.h"

using namespace std;

Test(ch05_test_suite, test_varint_encoding_decoding1) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);

  var_int = encode_variable_int(0, &int_len);
  cr_assert(int_len == 1);
  cr_assert(eq(u8, var_int[0], 0));  
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 0));
}

Test(ch05_test_suite, test_varint_encoding_decoding2) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);

  var_int = encode_variable_int(1234, &int_len);
  cr_assert(eq(u32, int_len, 3));
  uint8_t expected[] = {0xfd,0xd2,0x04};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 1234));
}

Test(ch05_test_suite, test_varint_encoding_decoding4) { 
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);

  var_int = encode_variable_int(1048575, &int_len);
  cr_assert(eq(u32, int_len, 5));
  uint8_t expected[] = {0xfe,0xff,0xff,0x0f,0x00};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 1048575));
}

Test(ch05_test_suite, test_varint_encoding_decoding5) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);
 
  var_int = encode_variable_int(31415926, &int_len);
  cr_assert(eq(u32, int_len, 5));
  uint8_t expected[] = {0xfe,0x76,0x5e,0xdf,0x01};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 31415926));
}

Test(ch05_test_suite, test_varint_encoding_decoding6) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);
  
  var_int = encode_variable_int(2147483648, &int_len);
  cr_assert(eq(u32, int_len, 5));
  uint8_t expected[] = {0xfe,0x00,0x00,0x00,0x80};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 2147483648));
}

Test(ch05_test_suite, test_varint_encoding_decoding7) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(9);

  var_int = encode_variable_int(4294967296, &int_len);
  cr_assert(eq(u32, int_len, 9));
  uint8_t expected[] = {0xff,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 4294967296));
}

Test(ch05_test_suite, test_varint_encoding_decoding8) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(9);

  var_int = encode_variable_int(18446744073709551615u, &int_len);; // 0xffffffffffffffffff, max value of uint64_t
  cr_assert(eq(u32, int_len, 9));
  uint8_t expected[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 18446744073709551615u));
}

Test(ch05_test_suite, test_parse1) {
  size_t input_len;
  vector<uint8_t> d;
  // The input hex string is from the book
  uint8_t* hex_input = hex_string_to_bytes(
    "0100000001813f79011acb80925dfe69b3def355fe914bd1d96a3f5f71bf8303c6a989c7d1000000006b483045022100ed81ff192e75a3fd2"
    "304004dcadb746fa5e24c5031ccfcf21320b0277457c98f02207a986d955c6e0cb35d446a89d3f56100f4d7f67801c31967743a9c8e10615b"
    "ed01210349fc4e631e3624a545de3f89f5d8684c7b8138bd94bdd531d2e213bf016b278afeffffff02a135ef01000000001976a914bc3b654"
    "dca7e56b04dca18f2566cdaf02e8d9ada88ac99c39800000000001976a9141c4bc762dd5423e332166702cb75f40df79fea1288ac19430600",
    &input_len
  );
  for (size_t i = 0; i < input_len; ++i) {
    d.push_back(hex_input[i]);
  }
  free(hex_input);  
  Tx my_tx = Tx();
  my_tx.parse(d);
  cr_assert(eq(u32, my_tx.get_version(), 1u));
  cr_assert(eq(u32, my_tx.get_tx_in_count(), 1u));
  vector<TxIn> tx_ins = my_tx.get_tx_ins();
  uint8_t* prev_tx_id = tx_ins[0].get_prev_tx_id();
  uint8_t expected[] = {0xd1,0xc7,0x89,0xa9,0xc6,0x03,0x83,0xbf,0x71,0x5f,0x3f,0x6a,0xd9,0xd1,0x4b,0x91,0xfe,0x55,0xf3,0xde,0xb3,0x69,0xfe,0x5d,0x92,0x80,0xcb,0x1a,0x01,0x79,0x3f,0x81};
  cr_assert(sizeof(expected)/sizeof(uint8_t) == SHA256_HASH_SIZE);
  for (int i = 0; i < SHA256_HASH_SIZE; ++i) {
    cr_assert(prev_tx_id[i] == expected[i]);
  }
  cr_assert(eq(u32, tx_ins[0].get_prev_tx_idx(), 0u));
  cr_assert(eq(u32, tx_ins[0].get_sequence(), 0xfffffffeu));

  cr_assert(eq(u32, my_tx.get_tx_out_count(), 2u));
  
  vector<TxOut> tx_outs = my_tx.get_tx_outs();
  cr_assert(eq(u64, tx_outs[0].get_amount(), 32454049u));
  cr_assert(eq(u64, tx_outs[1].get_amount(), 10011545));
  cr_assert(eq(u32, my_tx.get_locktime(), 410393u));
}

Test(ch05_test_suite, test_parse2) {
  size_t input_len;
  vector<uint8_t> d;
  uint8_t* hex_input = hex_string_to_bytes(
    "010000000456919960ac691763688d3d3bcea9ad6ecaf875df5339e148a1fc61c6ed7a069e010000006a47304402204585bcdef85e6b1c6af"
    "5c2669d4830ff86e42dd205c0e089bc2a821657e951c002201024a10366077f87d6bce1f7100ad8cfa8a064b39d4e8fe4ea13a7b71aa8180f"
    "012102f0da57e85eec2934a82a585ea337ce2f4998b50ae699dd79f5880e253dafafb7feffffffeb8f51f4038dc17e6313cf831d4f02281c2"
    "a468bde0fafd37f1bf882729e7fd3000000006a47304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc26125"
    "022008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a7160121035d5c93d9ac96881f19ba1f686f15f009ded7c62"
    "efe85a872e6a19b43c15a2937feffffff567bf40595119d1bb8a3037c356efd56170b64cbcc160fb028fa10704b45d775000000006a473044"
    "02204c7c7818424c7f7911da6cddc59655a70af1cb5eaf17c69dadbfc74ffa0b662f02207599e08bc8023693ad4e9527dc42c34210f7a7d1d"
    "1ddfc8492b654a11e7620a0012102158b46fbdff65d0172b7989aec8850aa0dae49abfb84c81ae6e5b251a58ace5cfeffffffd63a5e6c16e6"
    "20f86f375925b21cabaf736c779f88fd04dcad51d26690f7f345010000006a47304402200633ea0d3314bea0d95b3cd8dadb2ef79ea8331ff"
    "e1e61f762c0f6daea0fabde022029f23b3e9c30f080446150b23852028751635dcee2be669c2a1686a4b5edf304012103ffd6f4a67e94aba3"
    "53a00882e563ff2722eb4cff0ad6006e86ee20dfe7520d55feffffff0251430f00000000001976a914ab0c0b2e98b1ab6dbf67d4750b0a562"
    "44948a87988ac005a6202000000001976a9143c82d7df364eb6c75be8c80df2b3eda8db57397088ac46430600",
    &input_len
  );
  for (size_t i = 0; i < input_len; ++i) {
    d.push_back(hex_input[i]);
  }
  free(hex_input);

  Tx my_tx = Tx();
  my_tx.parse(d);

  vector<TxOut> tx_outs = my_tx.get_tx_outs();
  cr_assert(eq(u32, tx_outs[1].get_amount(), 40000000u));
}

Test(ch05_test_suite, test_parse_fee1) {
  vector<uint8_t> d;
  size_t input_len;
  uint8_t* hex_input = hex_string_to_bytes(
    "0100000001813f79011acb80925dfe69b3def355fe914bd1d96a3f5f71bf8303c6a989c7d1000000006b483045022100ed81ff192e75a3fd2"
    "304004dcadb746fa5e24c5031ccfcf21320b0277457c98f02207a986d955c6e0cb35d446a89d3f56100f4d7f67801c31967743a9c8e10615b"
    "ed01210349fc4e631e3624a545de3f89f5d8684c7b8138bd94bdd531d2e213bf016b278afeffffff02a135ef01000000001976a914bc3b654"
    "dca7e56b04dca18f2566cdaf02e8d9ada88ac99c39800000000001976a9141c4bc762dd5423e332166702cb75f40df79fea1288ac19430600",
    &input_len
  );
  for (size_t i = 0; i < input_len; ++i) {
    d.push_back(hex_input[i]);
  }
  free(hex_input);
  Tx my_tx = Tx();
  my_tx.parse(d);
  cr_assert(eq(u32, my_tx.get_fee(), 40000u));
}

Test(ch05_test_suite, test_parse_fee2) {
  vector<uint8_t> d;
  size_t input_len;
  uint8_t* hex_input = hex_string_to_bytes(
    "010000000456919960ac691763688d3d3bcea9ad6ecaf875df5339e148a1fc61c6ed7a069e010000006a47304402204585bcdef85e6b1c6a"
    "f5c2669d4830ff86e42dd205c0e089bc2a821657e951c002201024a10366077f87d6bce1f7100ad8cfa8a064b39d4e8fe4ea13a7b71aa818"
    "0f012102f0da57e85eec2934a82a585ea337ce2f4998b50ae699dd79f5880e253dafafb7feffffffeb8f51f4038dc17e6313cf831d4f0228"
    "1c2a468bde0fafd37f1bf882729e7fd3000000006a47304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc2"
    "6125022008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a7160121035d5c93d9ac96881f19ba1f686f15f009de"
    "d7c62efe85a872e6a19b43c15a2937feffffff567bf40595119d1bb8a3037c356efd56170b64cbcc160fb028fa10704b45d775000000006a"
    "47304402204c7c7818424c7f7911da6cddc59655a70af1cb5eaf17c69dadbfc74ffa0b662f02207599e08bc8023693ad4e9527dc42c34210"
    "f7a7d1d1ddfc8492b654a11e7620a0012102158b46fbdff65d0172b7989aec8850aa0dae49abfb84c81ae6e5b251a58ace5cfeffffffd63a"
    "5e6c16e620f86f375925b21cabaf736c779f88fd04dcad51d26690f7f345010000006a47304402200633ea0d3314bea0d95b3cd8dadb2ef7"
    "9ea8331ffe1e61f762c0f6daea0fabde022029f23b3e9c30f080446150b23852028751635dcee2be669c2a1686a4b5edf304012103ffd6f4"
    "a67e94aba353a00882e563ff2722eb4cff0ad6006e86ee20dfe7520d55feffffff0251430f00000000001976a914ab0c0b2e98b1ab6dbf67"
    "d4750b0a56244948a87988ac005a6202000000001976a9143c82d7df364eb6c75be8c80df2b3eda8db57397088ac46430600",
    &input_len
  );
  for (size_t i = 0; i < input_len; ++i) {
    d.push_back(hex_input[i]);
  }
  free(hex_input);
  Tx my_tx = Tx();
  my_tx.parse(d);  
  cr_assert(eq(u32, my_tx.get_fee(), 140500u));
}