#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "tx.h"
#include "utils.h"
#include "./cryptographic-algorithms/misc.h"

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
  Tx my_tx = Tx();
  my_tx.parse(&ss);
  printf("Function result: %u\n", my_tx.get_version());
  printf("Expected result: 1\n");
  printf("Function result: %u\n", my_tx.get_tx_in_count());
  printf("Expected result: 1\n");
  free(hex_input);
}

int main() {
  test_varint_encoding_decoding();
  test_parse();
  return 0;
}