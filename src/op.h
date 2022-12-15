#ifndef OP_H
#define OP_H

#include <stack>
#include <vector>

using namespace std;

typedef bool (*OpFunc)(stack<vector<uint8_t>>&);

struct OpFuncStruct {
  char func_name[32];
  OpFunc func_ptr ;   
}; 

OpFuncStruct get_opcode(size_t op_id);

#endif