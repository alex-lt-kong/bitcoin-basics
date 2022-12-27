#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdint.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "op.h"

using namespace std;


class Script {
private:
    vector<vector<uint8_t>> cmds;
    vector<bool> is_opcode;
    /*
        Store the nominal length (i.e., the length as specified by raw Bitcoin block byte stream) of the
        last operand. Following Bitcoin's Script serialization protocol is relatively straightforward until
        we reach the end of the byte stream. As the byte stream may be shorter than the length specified
        by the serialization protocol, creating the need of a few special treatment, including the
        introduction of this variable.
    */
    int last_operand_nominal_len; 
protected:
public:
    
    Script(vector<vector<uint8_t>> cmds);
    Script();
    /**
     * @brief Fill in the Script instance by parsing bytes from a stringstream. Script object has an is_nonstandard 
     * member variable, it will be set to true if parse() method consider the input bytes are non-standard.
     * @param d a vector from which bytes will be read from. Passing NULL/nullptr triggers undefined behaviors
     * @returns whether the stringstream is parsed successfully
     */
    bool parse(vector<uint8_t> d);
    /**
     * @brief Generate a series of bytes representing the cmds we parse()
     * @returns a vector contains bytes or an empty vector on error. Error info will be sent to stderr.
    */
    vector<uint8_t> serialize();
    /**
     * @brief get the parsed commands. Get command is either an opcode or an operand. This method should only be called
     * after parse() is successful; otherwise an empty vector is returned.
     * @returns the vector of commands
    */
    vector<vector<uint8_t>> get_cmds();
    /**
     * @brief the the vector of is_opcode. This size() of this vector is the same as cmds, is_opcode[idx] denotes
     * whether or not cmds[idx] is an opcode or an operand.
     * @returns the vector of is_opcode.
    */
    vector<bool> get_is_opcode();
    /**
     * @brief Convert the Script object to a human-readable string. The format will be the same as 
     * https://blockstream.info/api/tx/ in order to facilitate testing.
     * 
     * @return string 
     */
    string get_asm();
    /**
     * @brief If the parse() method is called and a non-standard Script is parsed, a private is_nonstandard member variable
     * will be set. Callers can query the status of it by calling this method.
    */
    ~Script();
};


#endif