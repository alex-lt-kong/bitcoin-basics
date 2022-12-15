#include "script.h"
#include "utils.h"

Script:: Script(vector<vector<uint8_t>> cmds) {
  this->cmds = cmds; // this makes a copy of cmds.
  this->is_nonstandard = false;
}

Script::Script() {
  this->is_nonstandard = false;
}

vector<uint8_t> Script::serialize() {
  vector<uint8_t> d(0);
  if (this->cmds.size() == 0) {
    fprintf(stderr, "this->cmds is empty, serialize() is not supposed to be called!\n");
    return vector<uint8_t>(0);
  }
  size_t idx = 0;
  while (idx < this->cmds.size()) {
    if (this->is_opcode[idx] == true) {
      if (this->cmds[idx][0] >= 78 || this->cmds[idx][0] == 0) {
        d.push_back(this->cmds[idx][0]);
        ++idx;
        continue;
      } else {
        fprintf(stderr, "Invalid opcode: %d\n", this->cmds[idx][0]);
        return vector<uint8_t>(0);
      }
    }
    size_t operand_len = this->cmds[idx].size();
    if (operand_len < 75) {
      d.push_back(operand_len);
      // data/operand does not need any extra bytes to encode, we directly push_back() operand_len
    }  
    else if (operand_len > 75 && operand_len < 256) {
      // data/operand needs one byte to encode, we need to encode 76 and then encode its real length
      d.push_back(76); // OP_PUSHDATA1
      d.push_back((uint8_t)operand_len); // there should be no wrapping.
    }
    else {
      /*
        data/operand needs two bytes to encode, we need to encode 76 and then encode its real length.
        In Jimmy Song's implementation, an exception will be thrown is operand_len > 520.
        This may make the client strictly follow the Bitcoin specs, but it may cause the client fail to parse some
        transactions on Bitcoin's main chain (such as this one:
        d29c9c0e8e4d2a9790922af73f0b8d51f0bd4bb19940d9cf910ead8fbe85bc9b).
        As a result, we don't enforce the rule here. Some relevant discussion:
        https://bitcoin.stackexchange.com/questions/78572/op-return-max-bytes-clarification
      */
      d.push_back(77); // OP_PUSHDATA2
      d.push_back((uint8_t)operand_len); // there should be wrapping, for 0x0A0B0C0D, it extracts 0D at 0
      d.push_back((uint8_t)(operand_len >> 8)); // there should be wrapping, for 0x0A0B0C0D, it extracts 0C at 1      
    }
    for (size_t j = 0; j < operand_len; ++j) {
      d.push_back(this->cmds[idx][j]);
    }
    ++idx;
  }
  size_t varint_len;
  uint8_t* varint_bytes_size = encode_variable_int(d.size(), &varint_len);
  for (int i = varint_len - 1; i >= 0; --i) {
    d.insert(d.begin(), varint_bytes_size[i]);
  }
  free(varint_bytes_size);
  return d;
}

bool Script::parse(vector<uint8_t>& d) {
  // https://en.bitcoin.it/wiki/Script
  uint64_t script_len = read_variable_int(d);
  size_t count = 0;
  uint8_t current = 0;
  size_t data_length = 0;
  this->cmds.clear();
  this->is_opcode.clear();
  this->is_nonstandard = false;
  while (count < script_len) {
    if (d.size() == 0) {
      fprintf(stderr, "script_len indicates there are more bytes to read but the bytes vector already empty!\n");
      return false;
    }
    current = d[0];
    d.erase(d.begin());
    ++ count;
    if (current >= 1 && current <= 75) {
      // an ordinary element should be between 1 to 75 bytes. If current is smaller then 75, it implies this component
      // is an ordinary element (i.e., data, not opcodes)
      if (current > d.size()) {
        fprintf(stderr, "WARNING: non-standard Script section detected: incorrect operand length\n");
        current = d.size();
        this->is_nonstandard = true;
      }
      vector<uint8_t> cmd(current);
      memcpy(cmd.data(), d.data(), current);
      d.erase(d.begin(), d.begin() + current);
      this->cmds.push_back(cmd);
      this->is_opcode.push_back(false);
      count += current;
    } else if (current == 76) {
      // 76 corresponds to OP_PUSHDATA1, meaning that we read the next byte, which specifies how many bytes
      // the element has.
      memcpy(&data_length, d.data(), 1);
      d.erase(d.begin());
      if (data_length > d.size()) {
        fprintf(stderr, "WARNING: non-standard Script section detected: incorrect operand length\n");
        data_length = d.size();
        this->is_nonstandard = true;
      }
      vector<uint8_t> cmd(data_length);
      memcpy(cmd.data(), d.data(), data_length);
      d.erase(d.begin(), d.begin() + data_length);
      this->cmds.push_back(cmd);
      this->is_opcode.push_back(false);
      count += data_length + 1;
    } else if (current == 77) {
      // 77 corresponds to OP_PUSHDATA2, meaning that we read the next 2 bytes
      // which, in little endian order, specify how many bytes the element has.
      uint8_t buf[2];
      memcpy(buf, d.data(), 2);
      d.erase(d.begin(), d.begin() + 2);
      data_length = buf[0] << 0 | buf[1] << 8; // little-endian bytes to int
      if (data_length > d.size()) {
        fprintf(stderr, "WARNING: non-standard Script section detected: incorrect operand length\n");
        data_length = d.size();
        this->is_nonstandard = true;
      }
      vector<uint8_t> cmd(data_length);
      memcpy(cmd.data(), d.data(), data_length);
      d.erase(d.begin(), d.begin() + data_length);
      this->cmds.push_back(cmd);
      this->is_opcode.push_back(false);
      count += data_length + 2;
    } else {
      // otherwise it is an opcode
      vector<uint8_t> cmd{ current };
      this->cmds.push_back(cmd);
      this->is_opcode.push_back(true);
    }
  }
  if (count != script_len) {
    this->cmds.clear();
     this->is_opcode.clear();
  }
  return count == script_len;
}

vector<vector<uint8_t>> Script::get_cmds() {
  return this->cmds;
}

vector<bool> Script::get_is_opcode() {
  return this->is_opcode;
}

string Script::get_asm() {
  string script_asm = "";
 /* for (size_t i = 0; i < this->cmds.size(); ++i) {
    if (this->is_opcode[i]) {
      script_asm += get_opcode(this->cmds[i][0]).func_name;
    } 
  }*/
  return script_asm;
}

bool Script::is_nonstandard_script_parsed() {
  return this->is_nonstandard;
}


Script::~Script() {}