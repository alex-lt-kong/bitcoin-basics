#include <unordered_map>
#include <stack>
#include <string>
#include <vector>

#include "./cryptographic-algorithms/src/sha256.h"

using namespace std;

typedef bool (*OpFunc)(stack<vector<uint8_t>>*);

bool op_dup(stack<vector<uint8_t>>* data_stack) {
  if (data_stack->empty()) {
    return false;
  }
  data_stack->push(data_stack->top());
  return true;
}

bool op_hash256(stack<vector<uint8_t>>* data_stack) {
  if (data_stack->empty()) {
    return false;
  }
  vector<uint8_t> ele = data_stack->top();
  data_stack->pop();
  uint8_t hash_bytes[SHA256_HASH_SIZE];
  cal_sha256_hash(ele.data(), ele.size(), hash_bytes);
  cal_sha256_hash(hash_bytes, SHA256_HASH_SIZE, hash_bytes);
  vector<uint8_t> hash(SHA256_HASH_SIZE);
  memcpy(&hash[0], &hash_bytes[0], SHA256_HASH_SIZE * sizeof(uint8_t));
  // fill a pre-allocated vector<uint8_t> with a byte array
  data_stack->push(hash);
  return true;
}

bool op_hash160(stack<vector<uint8_t>>* data_stack) {
  if (data_stack->empty()) {
    return false;
  }
  vector<uint8_t> ele = data_stack->top();
  data_stack->pop();
  uint8_t hash_bytes[RIPEMD160_HASH_SIZE];
  hash160(ele.data(), ele.size(), hash_bytes);
  vector<uint8_t> hash(RIPEMD160_HASH_SIZE);
  memcpy(&hash[0], &hash_bytes[0], RIPEMD160_HASH_SIZE * sizeof(uint8_t));
  // fill a pre-allocated vector<uint8_t> with a byte array
  data_stack->push(hash);
  return true;
}


unordered_map<int, OpFunc> Ops = {
    { 0x76, &op_dup },
    { 0xa9, &op_hash160 },
    { 0xaa, &op_hash256 }
};

