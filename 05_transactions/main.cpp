#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "tx.h"
#include "utils.h"

void test_varint_decoding() {
  printf("test_varint_decoding():\n");
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;

  var_int = encode_variable_int(1234);
  printf("Function result: ");
  for (int i = 0; i < 3; ++i) { printf("%#04x,", var_int[i]);}
  printf("\n");
  printf("Expected result: 0xfd,0xd2,0x04,\n");
  int_len = -1;
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %lld\n", result);
  printf("Expected result: 1234\n");

  var_int = encode_variable_int(31415926);
  printf("Function result: ");
  for (int i = 0; i < 5; ++i) { printf("%#04x,", var_int[i]);}
  printf("\n");
  printf("Expected result: 0xfe,0x76,0x5e,0xdf,0x01,\n");
  result = read_variable_int(var_int, &int_len);
  free(var_int);
  printf("Function result: %" PRIu64 "\n", result);
  printf("Expected result: 31415926\n");

  var_int = encode_variable_int(2147483648);
  printf("Function result: ");
  for (int i = 0; i < 5; ++i) { printf("%#04x,", var_int[i]);}
  printf("\n");
  printf("Expected result: 0xfe,0000,0000,0000,0x80,\n");
  result = read_variable_int(var_int, &int_len);
  printf("%d\n", int_len);
  free(var_int);
  printf("Function result: %lu\n", result);
  printf("Expected result: 31415926\n");
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
  test_varint_decoding();
  //test_parse();
  return 0;
}