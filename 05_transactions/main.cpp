#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "tx.h"
#include "utils.h"
#include "./cryptographic-algorithms/src/misc.h"

using namespace std;

void test_varint_encoding_decoding() {
  printf("test_varint_encoding_decoding():\n");
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  stringstream ss;
  var_int = encode_variable_int(0, &int_len);
  
  printf("Function result: ");
  for (size_t i = 0; i < int_len; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0x00,\n");
  ss.write((char*)var_int, int_len);
  result = read_variable_int(&ss);
  free(var_int);
  printf("Function result: %lu\n", result);
  printf("Expected result: 0\n");
  
  var_int = encode_variable_int(1234, &int_len);
  printf("Function result: ");
  for (size_t i = 0; i < int_len; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xfd,0xd2,0x04,\n");
  ss.write((char*)var_int, int_len);
  result = read_variable_int(&ss);
  free(var_int);
  printf("Function result: %lu\n", result);
  printf("Expected result: 1234\n");
  
  var_int = encode_variable_int(1048575, &int_len);
  printf("Function result: ");
  for (size_t i = 0; i < int_len; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xfe,0xff,0xff,0x0f,0000,\n");
  ss.write((char*)var_int, int_len);
  result = read_variable_int(&ss);
  free(var_int);
  printf("Function result: %ld\n", result);
  printf("Expected result: 1048575\n");
  
  var_int = encode_variable_int(31415926, &int_len);
  printf("Function result: ");
  for (size_t i = 0; i < int_len; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xfe,0x76,0x5e,0xdf,0x01,\n");
  ss.write((char*)var_int, int_len);
  result = read_variable_int(&ss);
  free(var_int);
  printf("Function result: %" PRIu64 "\n", result);
  printf("Expected result: 31415926\n");
  
  var_int = encode_variable_int(2147483648, &int_len);
  printf("Function result: ");
  for (size_t i = 0; i < int_len; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xfe,0000,0000,0000,0x80,\n");
  ss.write((char*)var_int, int_len);
  result = read_variable_int(&ss);
  free(var_int);
  printf("Function result: %lu\n", result);
  printf("Expected result: 2147483648\n");

  var_int = encode_variable_int(4294967296, &int_len);
  printf("Function result: ");
  for (size_t i = 0; i < int_len; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xff,0000,0000,0000,0000,0x01,0000,0000,0000,\n");
  ss.write((char*)var_int, int_len);
  result = read_variable_int(&ss);
  free(var_int);
  printf("Function result: %" PRIu64 "\n", result);
  printf("Expected result: 4294967296\n");

  var_int = encode_variable_int(18446744073709551615u, &int_len);; // 0xffffffffffffffffff, max value of uint64_t
  printf("Function result: ");
  for (size_t i = 0; i < int_len; ++i) { printf("%#04x,", var_int[i]);}
  printf("\n");
  printf("Expected result: 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,\n");
  ss.write((char*)var_int, int_len);
  result = read_variable_int(&ss);
  free(var_int);
  printf("Function result: %lu\n", result);
  printf("Expected result: 18446744073709551615\n");
  printf("\n");
}

void test_parse() {
  printf("test_parse():\n");
  size_t input_len;
  // The input hex string is from the book
  char* hex_input = (char*)hex_string_to_bytes(
    "0100000001813f79011acb80925dfe69b3def355fe914bd1d96a3f5f71bf8303c6a989c7d1000000006b483045022100ed81ff192e75a3fd2"
    "304004dcadb746fa5e24c5031ccfcf21320b0277457c98f02207a986d955c6e0cb35d446a89d3f56100f4d7f67801c31967743a9c8e10615b"
    "ed01210349fc4e631e3624a545de3f89f5d8684c7b8138bd94bdd531d2e213bf016b278afeffffff02a135ef01000000001976a914bc3b654"
    "dca7e56b04dca18f2566cdaf02e8d9ada88ac99c39800000000001976a9141c4bc762dd5423e332166702cb75f40df79fea1288ac19430600",
    &input_len
  );
  
  stringstream ss;
  ss.write(hex_input, input_len);
  free(hex_input);
  Tx my_tx = Tx();
  my_tx.parse(&ss);
  printf("Function get_version() result: %u\n", my_tx.get_version());
  printf("Expected get_version() result: 1\n");
  printf("Function get_tx_in_count() result: %u\n", my_tx.get_tx_in_count());
  printf("Expected get_tx_in_count() result: 1\n");
  vector<TxIn> tx_ins = my_tx.get_tx_ins();
  uint8_t* prev_tx_id = tx_ins[0].get_prev_tx_id();
  printf("Function tx_ins[0].get_prev_tx_id() result: ");
  for (int i = 0; i < SHA256_HASH_SIZE; ++i) {
    printf("%02x", prev_tx_id[i]);
  }
  printf("\n");
  printf("Expected tx_ins[0].get_prev_tx_id() result: d1c789a9c60383bf715f3f6ad9d14b91fe55f3deb369fe5d9280cb1a01793f81\n");
  printf("Function get_prev_tx_idx() result: %u\n", tx_ins[0].get_prev_tx_idx());
  printf("Expected get_prev_tx_idx() result: 0\n");
  printf("Function get_sequence() result: %#02x\n", tx_ins[0].get_sequence());
  printf("Expected get_sequence() result: 0xfffffffe\n");

  printf("Function get_tx_out_count() result: %u\n", my_tx.get_tx_out_count());
  printf("Expected get_tx_out_count() result: 2\n");

  vector<TxOut> tx_outs = my_tx.get_tx_outs();
  printf("Function tx_outs[0].get_amount() result: %lu\n", tx_outs[0].get_amount());
  printf("Expected tx_outs[0].get_amount() result: 32454049\n");
  printf("Function tx_outs[1].get_amount() result: %lu\n", tx_outs[1].get_amount());
  printf("Expected tx_outs[1].get_amount() result: 10011545\n");
  

  printf("Function my_tx.get_locktime() result: %u\n", my_tx.get_locktime());
  printf("Expected my_tx.get_locktime() result: 410393\n");

  hex_input = (char*)hex_string_to_bytes(
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
  ss.write(hex_input, input_len);
  free(hex_input);

  my_tx = Tx();
  my_tx.parse(&ss);

  tx_outs = my_tx.get_tx_outs();
  printf("Function tx_outs[1].get_amount() result: %lu\n", tx_outs[1].get_amount());
  printf("Expected tx_outs[1].get_amount() result: 40000000\n");

  printf("\n");
}

void test_parse_fee() {
  printf("test_parse_fee() (Note the function requires Internet connection to work):\n");
  size_t input_len;
  char* hex_input = (char*)hex_string_to_bytes(
    "0100000001813f79011acb80925dfe69b3def355fe914bd1d96a3f5f71bf8303c6a989c7d1000000006b483045022100ed81ff192e75a3fd2"
    "304004dcadb746fa5e24c5031ccfcf21320b0277457c98f02207a986d955c6e0cb35d446a89d3f56100f4d7f67801c31967743a9c8e10615b"
    "ed01210349fc4e631e3624a545de3f89f5d8684c7b8138bd94bdd531d2e213bf016b278afeffffff02a135ef01000000001976a914bc3b654"
    "dca7e56b04dca18f2566cdaf02e8d9ada88ac99c39800000000001976a9141c4bc762dd5423e332166702cb75f40df79fea1288ac19430600",
    &input_len
  );
  stringstream ss;
  ss.write(hex_input, input_len);  
  free(hex_input);
  Tx my_tx = Tx();
  my_tx.parse(&ss);  
  printf("Function my_tx.get_fee() result: %u\n", my_tx.get_fee());
  printf("Expected my_tx.get_fee() result: 40000\n");

  hex_input = (char*)hex_string_to_bytes(
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
  ss.write(hex_input, input_len);  
  free(hex_input);
  my_tx = Tx();
  my_tx.parse(&ss);  
  printf("Function my_tx.get_fee() result: %u\n", my_tx.get_fee());
  printf("Expected my_tx.get_fee() result: 140500\n");
}

int main() {
  test_varint_encoding_decoding();
  test_parse();
  test_parse_fee();
  return 0;
}