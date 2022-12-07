#include "script.h"
#include "utils.h"

Script:: Script(vector<vector<uint8_t>> cmds) {
  this->cmds = cmds; // this makes a copy of cmds.
}

Script::Script() {

}

bool Script::parse(vector<uint8_t>& d) {
  // https://en.bitcoin.it/wiki/Script
  uint64_t script_len = read_variable_int(d);
  size_t count = 0;
  uint8_t current = 0;
  size_t data_length = 0;
  this->cmds.clear();
  
  while (count < script_len) {
    current = d[0];
    d.erase(d.begin());
    ++ count;
    if (current >= 1 && current <= 75) {
      // an ordinary element should be between 1 to 75 bytes. If current is smaller then 75, it implies this component
      // is an ordinary element
      vector<uint8_t> cmd(current);
      memcpy(cmd.data(), d.data(), current);
      d.erase(d.begin(), d.begin() + current);
      this->cmds.push_back(cmd);
      count += current;
    } else if (current == 76) {
      // 76 corresponds to OP_PUSHDATA1, meaning that we read the next byte, which specifies how many bytes
      // the element has.
      memcpy(&data_length, d.data(), 1);
      d.erase(d.begin());
      vector<uint8_t> cmd(data_length);
      memcpy(cmd.data(), d.data(), data_length);
      d.erase(d.begin(), d.begin() + data_length);
      this->cmds.push_back(cmd);
      count += data_length + 1;
    } else if (current == 77) {
      // 77 corresponds to OP_PUSHDATA2, meaning that we read the next 2 bytes
      // which, in little endian order, specify how many bytes the element has.
      uint8_t buf[2];
      memcpy(buf, d.data(), 2);
      d.erase(d.begin(), d.begin() + 2);
      data_length = buf[0] << 0 | buf[1] << 8;
      vector<uint8_t> cmd(data_length);      
      memcpy(cmd.data(), d.data(), data_length);
      d.erase(d.begin(), d.begin() + data_length);
      this->cmds.push_back(cmd);
      count += data_length + 2;
    } else {
      vector<uint8_t> cmd{ current };
      this->cmds.push_back(cmd);
    }
  }
  return count == script_len;
}

vector<vector<uint8_t>> Script::get_cmds() {
  return this->cmds;
}

Script::~Script() {}