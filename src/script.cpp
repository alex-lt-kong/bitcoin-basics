#include <mycrypto/misc.h>

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
      } else if (this->cmds[idx][0] == 76) {
        d.push_back(this->cmds[idx][0]); // OP_PUSDATA1
        size_t operand_len = this->cmds[++idx].size();
        d.push_back((uint8_t)operand_len); // there should be no wrapping.
        for (size_t j = 0; j < operand_len && j < this->cmds[idx].size(); ++j) {
          d.push_back(this->cmds[idx][j]);
        }
        ++idx;
        continue;
      } else if (this->cmds[idx][0] == 77) {
        d.push_back(this->cmds[idx][0]); // OP_PUSDATA2
        size_t operand_len = this->cmds[++idx].size();
        d.push_back((uint8_t)operand_len); // there should be wrapping, for 0x0A0B0C0D, it extracts 0D at 0
        d.push_back((uint8_t)(operand_len >> 8)); // there should be wrapping, for 0x0A0B0C0D, it extracts 0C at 1
        for (size_t j = 0; j < operand_len && j < this->cmds[idx].size(); ++j) {
          d.push_back(this->cmds[idx][j]);
        }
        ++idx;
        continue;
      } else {
        fprintf(stderr, "Invalid opcode: %d\n", this->cmds[idx][0]);
        return vector<uint8_t>(0);
      }
    }
    size_t operand_len = this->cmds[idx].size();
    if (operand_len >= 75) {
      fprintf(stderr, "serialize()'ing non-standard cmds\n");
    }
    d.push_back(operand_len);
    for (size_t j = 0; j < operand_len && j < this->cmds[idx].size(); ++j) {
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
        fprintf(stderr, "Non-standard Script: incorrect operand length\n");
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
      this->cmds.push_back(vector<uint8_t>{ current });
      this->is_opcode.push_back(true);
      d.erase(d.begin());
      if (data_length > d.size()) {
        fprintf(stderr, "Non-standard Script: incorrect operand length\n");
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
      this->cmds.push_back(vector<uint8_t>{ current });
      this->is_opcode.push_back(true);
      data_length = buf[0] << 0 | buf[1] << 8; // little-endian bytes to int
      if (data_length > d.size()) {
        fprintf(stderr, "Non-standard Script: incorrect operand length\n");
        data_length = d.size();
        this->is_nonstandard = true;
      }
      vector<uint8_t> cmd(data_length);
      memcpy(cmd.data(), d.data(), data_length);
      d.erase(d.begin(), d.begin() + data_length);
      this->cmds.push_back(cmd);
      this->is_opcode.push_back(false);
      count += data_length + 2;
    } else if (current == 78) {
      // 77 corresponds to OP_PUSHDATA4, meaning that we read the next 4 bytes
      // which, in little endian order, specify how many bytes the element has.
      fprintf(stderr, "This branch has not been properly tested.\n");
      uint8_t buf[4];
      memcpy(buf, d.data(), 4);
      d.erase(d.begin(), d.begin() + 4);      
      this->cmds.push_back(vector<uint8_t>{ current });
      this->is_opcode.push_back(true);
      data_length = buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24; // little-endian bytes to int
      if (data_length > d.size()) {
        fprintf(stderr, "Non-standard Script: incorrect operand length\n");
        data_length = d.size();
        this->is_nonstandard = true;
      }
      vector<uint8_t> cmd(data_length);
      memcpy(cmd.data(), d.data(), data_length);
      d.erase(d.begin(), d.begin() + data_length);
      this->cmds.push_back(cmd);
      this->is_opcode.push_back(false);
      count += data_length + 4;
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
  char* hex_str;
  for (size_t i = 0; i < this->cmds.size(); ++i) {
    if (this->is_opcode[i]) {
      script_asm += get_opcode(this->cmds[i][0]).func_name;
      script_asm += " ";
      if (this->cmds[i][0] == 76 || this->cmds[i][0] == 77) {
        /*
        OP_PUSDATA1 and OP_PUSDATA2, will be loaded here, instead of relying on else {}.
        This design aims to make the output asm format consistent with:
        https://blockstream.info/api/tx/2f0d8d829a5e447eef46abb868de57924841755147a498b85deda841fc9b7889,
        which we rely on checking the parse(), serailize() and the get_asm()
        */
        ++i;
        if (
          (this->cmds[i].size() > 255 && this->cmds[i][0] == 76) ||
          (this->cmds[i].size() > 520 && this->cmds[i][0] == 77)
        ) {
          fprintf(stderr, "Non-standard Script: operand loaded by OP_PUSDATA has %lu bytes.\n", this->cmds[i].size());
        }
        hex_str = bytes_to_hex_string(this->cmds[i].data(), this->cmds[i].size(), false);
        script_asm += hex_str;
        script_asm += " ";
        free(hex_str);
      }
    } else {
      if (this->cmds[i].size() <= 75) {
        script_asm += "OP_PUSHBYTES_" + to_string(this->cmds[i].size()) + " ";
      } else {
        fprintf(stderr, "currently not supported\n");
        return "";
      }
      hex_str = bytes_to_hex_string(this->cmds[i].data(), this->cmds[i].size(), false);
      script_asm += hex_str;
      script_asm += " ";
      free(hex_str);
    }
    //printf("%s\n", script_asm.c_str());
  }
  script_asm.pop_back();
  return script_asm;
}

bool Script::is_nonstandard_script_parsed() {
  return this->is_nonstandard;
}


Script::~Script() {}
