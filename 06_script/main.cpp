#include <inttypes.h>
#include <stack>
#include <stdio.h>
#include <stdlib.h>

#include "tx.h"
#include "utils.h"
#include "script.h"
#include "op.h"
#include "./cryptographic-algorithms/src/misc.h"
#include "./cryptographic-algorithms/src/sha256.h"


using namespace std;

void test_op_dup() {
  printf("test_ops_dup():\n");
  stack<vector<uint8_t>> data_stack;
  data_stack.push(vector<uint8_t>{'H','e','l','l','o',' ','w','o','r','l','d','\0'});
  op_dup(&data_stack);
  op_dup(&data_stack);
  printf("Function data_stack.size() result: %ld\n", data_stack.size());
  printf("Expected data_stack.size() result: 3\n");
  printf("Function data_stack.top().c_str() result: ");
  while (data_stack.empty() == false) {
    printf("%s,", data_stack.top().data());
    data_stack.pop();
  }
  printf("\n");
  printf("Expected data_stack.top().c_str() result: Hello world,Hello world,Hello world,\n");
  bool ret_val = op_dup(&data_stack);
  printf("Function op_dup(&data_stack) result: %d\n", ret_val);
  printf("Expected op_dup(&data_stack) result: 0\n");
  printf("\n");
}

void test_op_hash256() {
  printf("test_op_hash256():\n");
  stack<vector<uint8_t>> data_stack;
  vector<uint8_t> hw_bytes = {'H','e','l','l','o',' ','w','o','r','l','d'};
  data_stack.push(hw_bytes);
  op_dup(&data_stack);
  op_hash256(&data_stack);
  printf("Function data_stack.size() result: %ld\n", data_stack.size());
  printf("Expected data_stack.size() result: 2\n");
  printf(
    "Function data_stack.top().data() result: %s\n", bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false)
  );
  printf(
    "Function data_stack.top().data() result: f6dc724d119649460e47ce719139e521e082be8a9755c5bece181de046ee65fe\n"
  );
  op_hash256(&data_stack);
  printf("Function data_stack.size() result: %ld\n", data_stack.size());
  printf("Expected data_stack.size() result: 2\n");
  printf(
    "Function data_stack.top().data() result: %s\n", bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false)
  );
  printf(
    "Function data_stack.top().data() result: 7a24c923419d8bd575e25f4f8e895d80648fb3ce2487de146e7a6ce5e7b6955d\n"
  );
  op_hash256(&data_stack);
  printf("Function data_stack.size() result: %ld\n", data_stack.size());
  printf("Expected data_stack.size() result: 2\n");
  printf(
    "Function data_stack.top().data() result: %s\n", bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false)
  );
  printf(
    "Function data_stack.top().data() result: f1f37d379df9bf407af43757558c7cb48fdd16d58b95d16df74573bd97c8f316\n"
  );
  printf("\n");
}

void test_op_hash160() {
  printf("test_op_hash160():\n");
  stack<vector<uint8_t>> data_stack;
  vector<uint8_t> hw_bytes = {'h','e','l','l','o',' ','w','o','r','l','d'};
  data_stack.push(hw_bytes);
  op_dup(&data_stack);
  op_hash160(&data_stack);
  printf("Function data_stack.size() result: %ld\n", data_stack.size());
  printf("Expected data_stack.size() result: 2\n");
  printf(
    "Function data_stack.top().data() result: %s\n", bytes_to_hex_string(data_stack.top().data(), RIPEMD160_HASH_SIZE, false)
  );
  printf("Function data_stack.top().data() result: d7d5ee7824ff93f94c3055af9382c86c68b5ca92\n");

  op_dup(&data_stack);
  op_hash160(&data_stack);
  printf("Function data_stack.size() result: %ld\n", data_stack.size());
  printf("Expected data_stack.size() result: 3\n");
  printf(
    "Function data_stack.top().data() result: %s\n", bytes_to_hex_string(data_stack.top().data(), RIPEMD160_HASH_SIZE, false)
  );
  printf("Function data_stack.top().data() result: f60b46dc1b792f72ba3b09b22cb48b592cc67b3b\n");
  printf("\n");
}

void test_script_parser() {
  printf("test_script_parser():\n");
  size_t input_len;
  // The input hex string is from the book
  char* hex_input = (char*)hex_string_to_bytes(
    "6a47304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc26125022008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a7160121035d5c93d9ac96881f19ba1f686f15f009ded7c62efe85a872e6a19b43c15a2937",
    &input_len
  );
  stringstream ss;
  ss.write(hex_input, input_len);
  free(hex_input);
  Script my_script = Script();
  bool ret_val = my_script.parse(&ss);
  printf("Function my_script.parse(&ss) result: %d\n", ret_val);
  printf("Expected my_script.parse(&ss) result: 1\n");
  printf("Function my_script.get_cmds().size() result: %ld\n", my_script.get_cmds().size());
  printf("Expected my_script.get_cmds().size() result: 2\n");
  vector<vector<uint8_t>> cmds = my_script.get_cmds();
  printf("Function cmds[0].data() result: %s\n", bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false));
  printf(
    "Expected cmds[0].data() result: "
    "304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc2612502"
    "2008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a71601\n"
  );
  printf("Function cmds[1].data() result: %s\n", bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false));
  printf("Expected cmds[1].data() result: 035d5c93d9ac96881f19ba1f686f15f009ded7c62efe85a872e6a19b43c15a2937\n");


  hex_input = (char*)hex_string_to_bytes(
    "1976a914cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc088ac",
    &input_len
  );
  // This test case is manually constructed from transaction:
  // d21736be48b88d15591b101ecadcba8f65713876ffb2b29d60de01dfaef8b120
  ss.clear();
  ss.write(hex_input, input_len);
  free(hex_input);
  my_script = Script();
  ret_val = my_script.parse(&ss);
  printf("Function my_script.parse(&ss) result: %d\n", ret_val);
  printf("Expected my_script.parse(&ss) result: 1\n");
  printf("Function my_script.get_cmds().size() result: %ld\n", my_script.get_cmds().size());
  printf("Expected my_script.get_cmds().size() result: 5\n");
  cmds = my_script.get_cmds();
  printf("Function cmds[0].data() result: %s\n", bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false));
  printf("Expected cmds[0].data() result: 76\n");
  printf("Function cmds[1].data() result: %s\n", bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false));
  printf("Expected cmds[1].data() result: a9\n");
  printf("Function cmds[2].data() result: %s\n", bytes_to_hex_string(cmds[2].data(), cmds[2].size(), false));
  printf("Expected cmds[2].data() result: cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc0\n");
  printf("Function cmds[3].data() result: %s\n", bytes_to_hex_string(cmds[3].data(), cmds[3].size(), false));
  printf("Expected cmds[3].data() result: 88\n");
  printf("Function cmds[4].data() result: %s\n", bytes_to_hex_string(cmds[4].data(), cmds[4].size(), false));
}

int main() {
  test_op_dup();
  test_op_hash256();
  test_op_hash160();
  test_script_parser();
  return 0;
}