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
  if (op_id > 186) {
    // Some silly/malicious clients could invoke this
    // we follow the format used by https://blockstream.info/api/tx/
    OpFuncStruct tmp = {"", &op_notimplemented};
    sprintf(tmp.func_name, "OP_RETURN_%ld", op_id);
    return tmp;
  }
  OpFuncStruct Ops[187];
  for (size_t i = 0; i < sizeof(Ops) / sizeof(Ops[0]); ++i) {
    Ops[i] = (OpFuncStruct){"OP_NOTIMPLEMENTED", &op_notimplemented};
  }
  Ops[  0] = (OpFuncStruct){"OP_0",                &op_0};
  Ops[ 76] = (OpFuncStruct){"OP_PUSHDATA1",        &op_notimplemented};
  Ops[ 77] = (OpFuncStruct){"OP_PUSHDATA2",        &op_notimplemented};
  Ops[ 81] = (OpFuncStruct){"OP_PUSHNUM_1",        &op_notimplemented};
  Ops[ 83] = (OpFuncStruct){"OP_PUSHNUM_3",        &op_notimplemented};
  Ops[ 86] = (OpFuncStruct){"OP_PUSHNUM_6",        &op_notimplemented};
  Ops[ 88] = (OpFuncStruct){"OP_PUSHNUM_8",        &op_notimplemented};
  Ops[ 91] = (OpFuncStruct){"OP_PUSHNUM_11",       &op_notimplemented};
  Ops[ 96] = (OpFuncStruct){"OP_PUSHNUM_16",       &op_notimplemented};
  Ops[106] = (OpFuncStruct){"OP_RETURN",           &op_notimplemented};
  Ops[107] = (OpFuncStruct){"OP_TOALTSTACK",       &op_notimplemented};
  Ops[108] = (OpFuncStruct){"OP_FROMALTSTACK",     &op_notimplemented};
  Ops[109] = (OpFuncStruct){"OP_2DROP",            &op_notimplemented};
  Ops[112] = (OpFuncStruct){"OP_2OVER",            &op_notimplemented};
  Ops[117] = (OpFuncStruct){"OP_DROP",             &op_notimplemented};
  Ops[118] = (OpFuncStruct){"OP_DUP",              &op_dup};
  Ops[119] = (OpFuncStruct){"OP_NIP",              &op_notimplemented};
  Ops[123] = (OpFuncStruct){"OP_ROT",              &op_notimplemented};
  Ops[125] = (OpFuncStruct){"OP_TUCK",             &op_notimplemented};    
  Ops[127] = (OpFuncStruct){"OP_SUBSTR",           &op_notimplemented};
  Ops[126] = (OpFuncStruct){"OP_CAT",              &op_notimplemented};  
  Ops[135] = (OpFuncStruct){"OP_EQUAL",            &op_notimplemented};
  Ops[136] = (OpFuncStruct){"OP_EQUALVERIFY",      &op_notimplemented};
  Ops[149] = (OpFuncStruct){"OP_MUL",              &op_notimplemented};  
  Ops[153] = (OpFuncStruct){"OP_RSHIFT",           &op_notimplemented};    
  Ops[158] = (OpFuncStruct){"OP_NUMNOTEQUAL",      &op_notimplemented};  
  Ops[163] = (OpFuncStruct){"OP_MIN",              &op_notimplemented};
  Ops[169] = (OpFuncStruct){"OP_HASH160",          &op_hash160};
  Ops[170] = (OpFuncStruct){"OP_HASH256",          &op_hash256};
  Ops[171] = (OpFuncStruct){"OP_CODESEPARATOR",    &op_notimplemented};
  Ops[172] = (OpFuncStruct){"OP_CHECKSIG",         &op_notimplemented};
  Ops[173] = (OpFuncStruct){"OP_CHECKSIGVERIFY",   &op_notimplemented};
  Ops[174] = (OpFuncStruct){"OP_CHECKMULTISIG",    &op_notimplemented};
  Ops[177] = (OpFuncStruct){"OP_CLTV",             &op_notimplemented}; // a.k.a. OP_CHECKLOCKTIMEVERIFY
  Ops[179] = (OpFuncStruct){"OP_NOP4",             &op_notimplemented};
  Ops[180] = (OpFuncStruct){"OP_NOP5",             &op_notimplemented};
  Ops[181] = (OpFuncStruct){"OP_NOP6",             &op_notimplemented};
  Ops[182] = (OpFuncStruct){"OP_NOP7",             &op_notimplemented};
  Ops[186] = (OpFuncStruct){"OP_CHECKSIGADD",      &op_notimplemented};
 
  return Ops[op_id];
}
