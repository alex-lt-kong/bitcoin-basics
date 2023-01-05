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
        If last_operand is preceded by an OP_PUSHDATA, last_operand INcludes
        the bytes used to specify the length of "real operand".
        Otherwise (i.e., last_operand is preceded by an fictional
        OP_PUSHBYTES_,), it INcludeds the preceding bytes denoting the length
        of the operand.
        Either way, we store the nominal length of the last operand in order
        to properly serialize() the Script to bytes etc.
    */
    vector<uint8_t> last_operand;
    /**
     * @brief get the number of bytes next to OP_PUSHDATA operations to store
     * the length of the incoming operand. Note that this method only returns
     * the number of bytes, not the value represented by these bytes.
     * For example, 0x02 is one-byte long but it represents 2.
     * @param opcode opcode, can only be 76, 77 or 78 per Bitcoin's specs
     * @returns the number of bytes used to store operand's length
     * (i.e., 1, 2 or 4) or -1 if an invalid opcode is passed
    */
    int get_nominal_operand_len_byte_count_after_op_pushdata(uint8_t opcode);
    /**
     * @brief Get the nominal operand len after an OP_PUSHDATA opcode, EXcluding
     * the bytes used to store the length of the operand
     * 
     * @param opcode can only be 76, 77 or 78 per Bitcoin's specs
     * @param byte_stream the raw bytes series, can be shorter than specified
     * by Bitcoin's spec.
     * @return the nominal length of the operand or -1 if an invalid opcode is 
     * passed.
     */
    int64_t get_nominal_operand_len_after_op_pushdata(
        uint8_t opcode, vector<uint8_t> byte_stream);
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