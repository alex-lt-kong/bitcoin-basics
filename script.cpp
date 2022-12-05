#include "script.h"
#include "utils.h"

Script:: Script(vector<vector<uint8_t>> cmds) {
  this->cmds = cmds; // this makes a copy of cmds.
}

Script::Script() {

}

bool Script::parse(stringstream* ss) {
  uint64_t script_len = read_variable_int(ss);
  size_t count = 0;
  uint8_t current = 0;
  size_t data_length = 0;
  this->cmds.clear();
  
  while (count < script_len) {
    ss->read((char*)(&current), 1);
    ++ count;
    if (current >= 1 && current <= 75) { // Element
      vector<uint8_t> cmd(current);
      ss->read((char*)cmd.data(), current);
      this->cmds.push_back(cmd);
      count += current;
    } else if (current == 76) {
      printf("WARNING: this route is never tested!\n");
      ss->read((char*)(&data_length), 1);
      vector<uint8_t> cmd(data_length);
      ss->read((char*)cmd.data(), data_length);
      this->cmds.push_back(cmd);
      count += data_length + 1;
    } else if (current == 77) {
      printf("WARNING: this route is never tested!\n");
      uint8_t buf[2];
      ss->read((char*)(buf), 2);
      data_length = buf[0] << 0 | buf[1] << 8;
      vector<uint8_t> cmd(data_length);
      ss->read((char*)cmd.data(), data_length);
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