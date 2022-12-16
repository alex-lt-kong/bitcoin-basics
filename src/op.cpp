#include <stack>
#include <string>
#include <vector>

#include "../cryptographic-algorithms/src/sha256.h"
#include "utils.h"


// Description of Script opcodes: https://en.bitcoin.it/wiki/Script
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
  if (op_id > 256) {
    return (OpFuncStruct){"OP_NOTIMPLEMENTED", &op_notimplemented};
  }
  if (op_id > 185) {
    // Some silly/malicious clients could invoke this
    // we follow the format used by https://blockstream.info/api/tx/
    OpFuncStruct tmp = {"", &op_notimplemented};
    sprintf(tmp.func_name, "OP_RETURN_%ld", op_id);
    return tmp;
  }
  OpFuncStruct Ops[186];
  for (size_t i = 0; i < sizeof(Ops) / sizeof(Ops[0]); ++i) {
    Ops[i] = (OpFuncStruct){"OP_NOTIMPLEMENTED", &op_notimplemented};
  }
  Ops[  0] = (OpFuncStruct){"OP_0",                &op_0};
  Ops[ 76] = (OpFuncStruct){"OP_PUSHDATA1",        &op_notimplemented};
  Ops[ 77] = (OpFuncStruct){"OP_PUSHDATA2",        &op_notimplemented};
  Ops[ 81] = (OpFuncStruct){"OP_PUSHNUM_1",        &op_notimplemented};
  Ops[106] = (OpFuncStruct){"OP_RETURN",           &op_notimplemented};
  Ops[109] = (OpFuncStruct){"OP_2DROP",            &op_notimplemented};
  Ops[118] = (OpFuncStruct){"OP_DUP",              &op_dup};
  Ops[135] = (OpFuncStruct){"OP_EQUAL",            &op_notimplemented};
  Ops[136] = (OpFuncStruct){"OP_EQUALVERIFY",      &op_notimplemented};
  Ops[163] = (OpFuncStruct){"OP_MIN",              &op_notimplemented};
  Ops[169] = (OpFuncStruct){"OP_HASH160",          &op_hash160};
  Ops[170] = (OpFuncStruct){"OP_HASH256",          &op_hash256};
  Ops[171] = (OpFuncStruct){"OP_CODESEPARATOR",    &op_notimplemented};
  Ops[172] = (OpFuncStruct){"OP_CHECKSIG",         &op_notimplemented};
  Ops[173] = (OpFuncStruct){"OP_CHECKSIGVERIFY",   &op_notimplemented};
 
  return Ops[op_id];
}
