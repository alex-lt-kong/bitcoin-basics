#include <inttypes.h>
#include <stack>
#include <stdio.h>
#include <stdlib.h>

#include "tx.h"
#include "utils.h"
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

int main() {
  test_op_dup();
  test_op_hash256();
  test_op_hash160();
  return 0;
}