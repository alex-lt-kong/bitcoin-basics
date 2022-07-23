#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "tx.h"
#include "utils.h"

void test_varint_encoding_decoding() {
  printf("test_varint_encoding_decoding():\n");
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;

  var_int = encode_variable_int(0);
  printf("Function result: ");
  for (int i = 0; i < 1; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0x00,\n");
  int_len = -1;
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %lld\n", result);
  printf("Expected result: 0\n");

  var_int = encode_variable_int(1234);
  printf("Function result: ");
  for (int i = 0; i < 3; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xfd,0xd2,0x04,\n");
  int_len = -1;
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %lld\n", result);
  printf("Expected result: 1234\n");

  var_int = encode_variable_int(1048575);
  printf("Function result: ");
  for (int i = 0; i < 5; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xfe,0xff,0xff,0x0f,0000,\n");
  int_len = -1;
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %lld\n", result);
  printf("Expected result: 1048575\n");

  var_int = encode_variable_int(31415926);
  printf("Function result: ");
  for (int i = 0; i < 5; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xfe,0x76,0x5e,0xdf,0x01,\n");
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %" PRIu64 "\n", result);
  printf("Expected result: 31415926\n");

  var_int = encode_variable_int(2147483648);
  printf("Function result: ");
  for (int i = 0; i < 5; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xfe,0000,0000,0000,0x80,\n");
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %lu\n", result);
  printf("Expected result: 2147483648\n");

  var_int = encode_variable_int(4294967296);
  printf("Function result: ");
  for (int i = 0; i < 9; ++i) { printf("%#04x,", var_int[i]); }
  printf("\n");
  printf("Expected result: 0xff,0000,0000,0000,0000,0x01,0000,0000,0000,\n");
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %" PRIu64 "\n", result);
  printf("Expected result: 4294967296\n");

  var_int = encode_variable_int(18446744073709551615u); // 0xffffffffffffffffff, max value of uint64_t
  printf("Function result: ");
  for (int i = 0; i < 9; ++i) { printf("%#04x,", var_int[i]);}
  printf("\n");
  printf("Expected result: 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,\n");
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %lu\n", result);
  printf("Expected result: 18446744073709551615\n");
}

void test_parse() {
  uint8_t* data = (uint8_t*)calloc(4, sizeof(uint8_t));
  data[0] = 1;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  Tx::parse(data);
  free(data);
}

int main() {
  test_varint_encoding_decoding();
  //test_parse();
  return 0;
}