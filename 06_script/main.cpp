#include <inttypes.h>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "tx.h"
#include "utils.h"
#include "script.h"
#include "op.h"
#include "./cryptographic-algorithms/src/misc.h"
#include "./cryptographic-algorithms/src/sha256.h"


using namespace std;

void test_op_dup() {
  stack<vector<uint8_t>> data_stack;
  data_stack.push(vector<uint8_t>{'H','e','l','l','o',' ','w','o','r','l','d','\0'});
  op_dup(&data_stack);
  op_dup(&data_stack);
  cr_expect(data_stack.size() == 3);
  while (data_stack.empty() == false) {
    cr_expect(eq(str, (char*)data_stack.top().data(), "Hello world"));
    data_stack.pop();
  }
  bool ret_val = op_dup(&data_stack);
  cr_expect(ret_val == 0);
}

void test_op_hash256() {
  stack<vector<uint8_t>> data_stack;
  vector<uint8_t> hw_bytes = {'H','e','l','l','o',' ','w','o','r','l','d'};
  data_stack.push(hw_bytes);
  op_dup(&data_stack);
  op_hash256(&data_stack);
  cr_expect(data_stack.size() == 2);
  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false),
      "f6dc724d119649460e47ce719139e521e082be8a9755c5bece181de046ee65fe"
    )
  );
  op_hash256(&data_stack);
  cr_expect(data_stack.size() == 2);
  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false),
      "7a24c923419d8bd575e25f4f8e895d80648fb3ce2487de146e7a6ce5e7b6955d"
    )
  );
  op_hash256(&data_stack);
  cr_expect(data_stack.size() == 2);
  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false),
      "f1f37d379df9bf407af43757558c7cb48fdd16d58b95d16df74573bd97c8f316"
    )
  );
}

void test_op_hash160() {
  stack<vector<uint8_t>> data_stack;
  vector<uint8_t> hw_bytes = {'h','e','l','l','o',' ','w','o','r','l','d'};
  data_stack.push(hw_bytes);
  op_dup(&data_stack);
  op_hash160(&data_stack);
  cr_expect(data_stack.size() == 2);
  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), RIPEMD160_HASH_SIZE, false),
      "d7d5ee7824ff93f94c3055af9382c86c68b5ca92"
    )
  );

  op_dup(&data_stack);
  op_hash160(&data_stack);
  cr_expect(data_stack.size() == 3);

  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), RIPEMD160_HASH_SIZE, false),
      "f60b46dc1b792f72ba3b09b22cb48b592cc67b3b"
    )
  );
}

void test_script_parser() {
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
  cr_expect(ret_val == 1);
  cr_expect(my_script.get_cmds().size() == 2);
  vector<vector<uint8_t>> cmds = my_script.get_cmds();
  cr_expect(
    eq(
      str, bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false),
      "304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc2612502"
      "2008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a71601"
    )
  );
  cr_expect(
    eq(
      str, bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false),
      "035d5c93d9ac96881f19ba1f686f15f009ded7c62efe85a872e6a19b43c15a2937"
    )
  );

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
  cr_expect(ret_val == 1);
  cr_expect(my_script.get_cmds().size() == 5);
  cmds = my_script.get_cmds();
  cr_expect(eq(str, bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false), "76"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false), "a9"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[2].data(), cmds[2].size(), false), "cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc0"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[3].data(), cmds[3].size(), false), "88"));
  printf("Function cmds[4].data() result: %s\n", bytes_to_hex_string(cmds[4].data(), cmds[4].size(), false));
  
}


Test(ch06_test_suite, test_op_dup) {
  test_op_dup();
}


Test(ch06_test_suite, test_op_hash256) {
  test_op_hash256();
}

Test(ch06_test_suite, test_op_hash160) {
  test_op_hash160();
}

Test(ch06_test_suite, test_script_parser) {
  test_script_parser();
}