#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdint.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;


class Script {
private:
  vector<vector<uint8_t>> cmds;
protected:
public:
  
  Script(vector<vector<uint8_t>> cmds);
  Script();
  /**
   * @brief Fill in the Script instance by parsing bytes from a stringstream.
   * @param d a vector from which bytes will be read from. Passing NULL/nullptr triggers undefined behaviors
   * @returns whether the stringstream is parsed successfully
   */
  bool parse(vector<uint8_t>& d);
  /**
   * @brief Generate a series of bytes representing the cmds we parse()
   * @returns a vector contains bytes or an empty vector on error. Error info will be sent to stderr.
  */
  vector<uint8_t> serialize();
  vector<vector<uint8_t>> get_cmds();
  ~Script();
};


#endif