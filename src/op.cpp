#include <stack>
#include <string>
#include <vector>

#include "../cryptographic-algorithms/src/sha256.h"
#include "utils.h"

using namespace std;


typedef bool (*OpFunc)(stack<vector<uint8_t>>&);

struct OpFuncStruct {
  char func_name[32];
  OpFunc func_ptr ;   
}; 

bool op_notimplemented(stack<vector<uint8_t>>& data_stack) {
  return false;
}

bool op_0(stack<vector<uint8_t>>& data_stack) {
  return false;
}

bool op_dup(stack<vector<uint8_t>>& data_stack) {
  if (data_stack.empty()) {
    return false;
  }
  data_stack.push(data_stack.top());
  return true;
}

bool op_hash256(stack<vector<uint8_t>>& data_stack) {
  if (data_stack.empty()) {
    return false;
  }
  vector<uint8_t> ele = data_stack.top();
  data_stack.pop();
  uint8_t hash_bytes[SHA256_HASH_SIZE];
  cal_sha256_hash(ele.data(), ele.size(), hash_bytes);
  cal_sha256_hash(hash_bytes, SHA256_HASH_SIZE, hash_bytes);
  vector<uint8_t> hash(SHA256_HASH_SIZE);
  memcpy(hash.data(), hash_bytes, SHA256_HASH_SIZE * sizeof(uint8_t));
  data_stack.push(hash);
  return true;
}

bool op_hash160(stack<vector<uint8_t>>& data_stack) {
  if (data_stack.empty()) {
    return false;
  }
  vector<uint8_t> ele = data_stack.top();
  // #include'ing ripemd160 introduce a conflict...so we use 20 instead of RIPEMD160_HASH_SIZE
  data_stack.pop();
  uint8_t hash_bytes[20];
  hash160(ele.data(), ele.size(), hash_bytes);
  vector<uint8_t> hash(20);
  memcpy(hash.data(), hash_bytes, 20 * sizeof(uint8_t));
  data_stack.push(hash);
  return true;
}


OpFuncStruct get_opcode(size_t op_id) {
  if (op_id > 185) {
    return (OpFuncStruct){"op_notimplemented", &op_notimplemented};
  }
  OpFuncStruct Ops[186] = {{"op_notimplemented", &op_notimplemented}};

  Ops[  0] = (OpFuncStruct){"op_0",       &op_0};
  Ops[118] = (OpFuncStruct){"op_dup",     &op_dup};
  Ops[169] = (OpFuncStruct){"op_hash160", &op_hash160};
  Ops[170] = (OpFuncStruct){"op_hash256", &op_hash256};

  return Ops[op_id];
}
