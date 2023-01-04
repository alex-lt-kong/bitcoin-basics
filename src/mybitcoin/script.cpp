#include <mycrypto/misc.h>

#include "script.h"
#include "utils.h"

Script::Script(vector<vector<uint8_t>> cmds) {
    this->cmds = cmds; // this makes a copy of cmds.
    last_operand_nominal_len = 0;
}

Script::Script() {}


vector<uint8_t> Script::serialize() {
    vector<uint8_t> d(0);
    if (cmds.size() == 0) {
        fprintf(stderr, "cmds is empty, serialize() is not supposed to be called!\n");
        return vector<uint8_t>(0);
    }
    size_t idx = 0;
    while (idx < cmds.size()) {
        if (is_opcode[idx] == true) {
            if (cmds[idx].size() != 1) {
                fprintf(stderr, "Fatal error: opcode occupied more "
                        "than one byte, which is supposed to be impossible\n");
                return vector<uint8_t>(0);
            }

            if (cmds[idx][0] > 78 || cmds[idx][0] == 0) {
                d.push_back(cmds[idx][0]);
            } else if (cmds[idx][0] >= 76 && cmds[idx][0] <= 78) {
                // raw bytes: 4c 01 0a
                d.push_back(cmds[idx][0]);

                ++idx;
                size_t operand_len = 0;
                if (idx == cmds.size() - 1) {
                    operand_len = last_operand_nominal_len;
                    operand_len = operand_len - get_op_pushdata_size(cmds[idx-1][0]) > 0 ? operand_len - get_op_pushdata_size(cmds[idx-1][0]) : 0;
                } else {
                    operand_len = cmds[idx].size();
                }
                if (is_opcode[idx] == true) {
                    /* Use to handle this scenario:
                        ASM:      OP_BOOLOR OP_PUSHDATA1    OP_0 OP_CODESEPARATOR
                        Actual:          9b           4c 01   00               ab
                        Expect:          9b           4c 00   00               ab
                        */
                    operand_len = 0;
                }
                // TODO: this section is very ugly, should re-write!
                if ( idx != cmds.size() - 1 ||
                    (idx == cmds.size() - 1 &&
                     last_operand_nominal_len > 0)
                ) {
                    d.push_back((uint8_t)operand_len);                 // for 0x0A0B0C0D, it extracts 0D at 0
                    if (cmds[idx-1][0] >= 77 && (
                        ( idx != cmds.size() - 1 ||
                         (idx == cmds.size() - 1 &&
                          last_operand_nominal_len > 1)))
                       ) {
                        d.push_back((uint8_t)(operand_len >> 8));      // for 0x0A0B0C0D, it extracts 0C at 1
                        if (cmds[idx-1][0] >= 78 && (
                        ( idx != cmds.size() - 1 ||
                         (idx == cmds.size() - 1 &&
                          last_operand_nominal_len > 2)))
                       ) {
                            d.push_back((uint8_t)(operand_len >> 16)); // for 0x0A0B0C0D, it extracts 0B at 2
                            if ( idx != cmds.size() - 1 ||
                                (idx == cmds.size() - 1 &&
                                last_operand_nominal_len > 3)) {
                            d.push_back((uint8_t)(operand_len >> 24)); // for 0x0A0B0C0D, it extracts 0A at 3
                            }
                        }
                    }
                }
                if (operand_len != cmds[idx].size()) {
                    fprintf(stderr, "Non-standard Script: operand_len is "
                            "different from operand.size()\n");
                }
                for (size_t j = 0; j < operand_len && j < cmds[idx].size(); ++j) {
                    d.push_back(cmds[idx][j]);
                }
                if (is_opcode[idx] == true) {
                    /*
                    It is a bit difficult to explain the case succinctly,
                    it is needed to parse the following test case:
                    "03cf760b1b4d696e656420627920416e74506f6f6c383738be00010045bd3903fabe6d6d8dee9c6ded1bbc251c0bdf56784cd8e32dc6c6448186a124ffdda854c54ff1eb02000000000000009b4c0000abc8000000000000"
                    "OP_PUSHBYTES_3 cf760b OP_PUSHBYTES_27 4d696e656420627920416e74506f6f6c383738be00010045bd3903 OP_RETURN_250 OP_RETURN_190 OP_2DROP OP_2DROP OP_2MUL OP_RETURN_238 OP_NUMEQUAL OP_2DROP OP_RETURN_237 OP_PUSHBYTES_27 bc251c0bdf56784cd8e32dc6c6448186a124ffdda854c54ff1eb02 OP_0 OP_0 OP_0 OP_0 OP_0 OP_0 OP_0 OP_BOOLOR OP_PUSHDATA1 OP_0 OP_CODESEPARATOR OP_RETURN_200 OP_0 OP_0 OP_0 OP_0 OP_0 OP_0"
                    */
                    --idx;
                }
            } else {
                fprintf(stderr, "Invalid opcode: %d\n", cmds[idx][0]);
                return vector<uint8_t>(0);
            }
        } else {
            size_t operand_len = (cmds.size() - 1) == idx ? 
            last_operand_nominal_len : cmds[idx].size();
            if (operand_len >= 75) {
                fprintf(stderr, "Non-standard Script: operand longer than "
                "75 bytes without OP_PUSHDATA\n");
            }
            d.push_back(operand_len);
            for (size_t j = 0; j < cmds[idx].size(); ++j) {
                d.push_back(cmds[idx][j]);
            }
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

bool Script::parse(vector<uint8_t> byte_stream) {
    // https://en.bitcoin.it/wiki/Script
    uint64_t script_len = read_variable_int(byte_stream);
    size_t count = 0;
    uint8_t cb = 0; // current byte
    size_t data_length = 0;
    cmds.clear();
    is_opcode.clear();
    const char cmd_names[][13] = {"OP_PUSHDATA1", "OP_PUSHDATA2", "OP_PUSHDATA4"};
    while (count < script_len) {
        if (byte_stream.size() == 0) {
            fprintf(stderr, "cbytes vector empty already\n");
            return false;
        }
        cb = byte_stream[0];
        byte_stream.erase(byte_stream.begin());
        ++ count;
        if (cb >= 1 && cb <= 75) {
            // an ordinary element should be between 1 to 75 bytes. If cb is smaller then 75, it implies this component
            // is an ordinary element (i.e., an operand, not an opcode)
            last_operand_nominal_len = cb;
            if (cb > byte_stream.size()) {
                // Will only enter this branch if the current operand is the last one.
                fprintf(stderr, "Non-standard Script: push past end\n");
                cb = byte_stream.size();
            }
            vector<uint8_t> cmd(cb);
            memcpy(cmd.data(), byte_stream.data(), cb);
            byte_stream.erase(byte_stream.begin(), byte_stream.begin() + cb);
            /*
                What if cb == 0?
                According to: https://en.cppreference.com/w/cpp/container/vector/erase
                If last == end() prior to removal, then the updated end() iterator is returned.
                If [first, last) is an empty range, then last is returned. 

                end() returns an iterator referring to the past-the-end element in the vector container. The
                past-the-end element is the theoretical element that would follow the last element in the vector.
                It does not point to any element, and thus shall not be dereferenced. 

                Therefore, it seems that cb == 0 and d.begin() == end() are both valid.
            */
            cmds.push_back(cmd);
            is_opcode.push_back(false);
            count += cb; // curr_byte stores the size of the operand
        } else if (cb >= 76 && cb <= 78) {
            // 76,77,78 corresponds to OP_PUSHDATA1/OP_PUSHDATA2/OP_PUSHDATA4,
            // meaning that we read the next 1,2,4 bytes
            // which, in little endian order, specify how many bytes the element has.
            size_t OP_PUSHDATA_size = (
                byte_stream.size() > (size_t)get_op_pushdata_size(cb) ?
                get_op_pushdata_size(cb) : byte_stream.size()
            );
            if ((int)OP_PUSHDATA_size < get_op_pushdata_size(cb)) {
                // Will only enter this branch if the coming operand is the last one.
                // It also implies that OP_PUSHDATA pushes nothing at all! (as it has already reached the end of d.)
                fprintf(stderr, "Non-standard Script: %lu too short for %s and "
                        "it pushes no data at all\n", byte_stream.size(),
                        cmd_names[cb-76]);
            }
            uint8_t buf[4] = {0};
            memcpy(buf, byte_stream.data(), OP_PUSHDATA_size);
            byte_stream.erase(
                byte_stream.begin(), byte_stream.begin() + OP_PUSHDATA_size
            );
            // As documented above, OP_PUSHDATA_size == 0 and byte_stream.begin() == end() are both valid.
            cmds.push_back(vector<uint8_t>{ cb });
            is_opcode.push_back(true);
            data_length = buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
            last_operand_nominal_len = OP_PUSHDATA_size + data_length;
            // little-endian bytes to int, this can be shared by three OP_PUSHDATAs
            if (data_length > byte_stream.size()) {
                // Will only enter this branch if the coming operand is the last one.
                fprintf(stderr, "Non-standard Script: push past end\n");
                data_length = byte_stream.size();
            }
            if (data_length > 520) {
                fprintf(stderr, "Non-standard Script: data_length > 520\n");
            }
            vector<uint8_t> cmd(data_length);
            memcpy(cmd.data(), byte_stream.data(), data_length);
            byte_stream.erase(
                byte_stream.begin(), byte_stream.begin() + data_length
            );
            cmds.push_back(cmd);
            is_opcode.push_back(false);
            
            count += data_length + OP_PUSHDATA_size;
        } else {
            // otherwise it is an opcode
            vector<uint8_t> cmd{ cb };
            cmds.push_back(cmd);
            is_opcode.push_back(true);
        }
    }
    return true;
}

vector<vector<uint8_t>> Script::get_cmds() {
    return cmds;
}

vector<bool> Script::get_is_opcode() {
    return is_opcode;
}

int Script::get_op_pushdata_size(uint8_t opcode) {
    const size_t operand_lengths[] = {1, 2, 4};
    if (opcode < 76 || opcode > 78) {
        return -1;
    }
    return operand_lengths[opcode - 76];
}


string Script::get_asm() {
    string script_asm = "";
    char* hex_str;
    if (cmds.size() == 0) {
        fprintf(stderr, "cmds is empty, get_asm() is not "
                "supposed to be called!\n");
    }
    for (size_t i = 0; i < cmds.size(); ++i) {
        if (is_opcode[i]) {
            script_asm += get_opcode(cmds[i][0]).func_name;
            script_asm += " ";
            if (cmds[i][0] >= 76 && cmds[i][0] <= 78) {
                /*
                OP_PUSDATA1, OP_PUSDATA2 and OP_PUSHDATA4, will be loaded here, instead of relying on else {}.
                This design aims to make the output asm format consistent with:
                https://blockstream.info/api/tx/2f0d8d829a5e447eef46abb868de57924841755147a498b85deda841fc9b7889,
                which we rely on checking the parse(), serailize() and the get_asm()
                */
                if (i+1 >= cmds.size() || is_opcode[i+1]) {
                    // Case I:  cmds reaches its end
                    // Case II: next cmd is still an opcode, not data.
                    fprintf(stderr, "Non-standard Script: OP_PUSHDATA followed by no data\n");
                    continue;
                }
                ++i;
                if ((cmds[i].size() > 255 && cmds[i-1][0] == 76) ||
                    (cmds[i].size() > 520 && cmds[i-1][0] >= 77)) {
                    fprintf(stderr, "Non-standard Script: operand loaded by "
                            "OP_PUSHDATA has %lu bytes.\n", cmds[i].size());
                }
                if (i == cmds.size() - 1) {
                    if (last_operand_nominal_len < (size_t)get_op_pushdata_size(cmds[i-1][0])) {
                        script_asm = script_asm.substr(0, script_asm.size() - strlen(" OP_PUSHDATA_ "));
                        script_asm += "<unexpected end>";
                        continue;
                    } else if (last_operand_nominal_len - 
                               get_op_pushdata_size(cmds[i-1][0]) >
                               cmds[i].size()) {
                        script_asm += "<push past end>"; 
                        continue;
                    }
                }
                hex_str = bytes_to_hex_string(cmds[i].data(), cmds[i].size(), false);
                script_asm += hex_str;
                script_asm += " ";
                free(hex_str);
            }
        } else {
            if (cmds[i].size() > 75) {
                fprintf(stderr, "This should never happen\n");
                return "";
            }            
            if (i == cmds.size() - 1 && cmds[i].size() != (size_t)last_operand_nominal_len) {
                script_asm += "OP_PUSHBYTES_" + to_string(last_operand_nominal_len) + " ";
                script_asm += "<push past end>";
            } else {
                script_asm += "OP_PUSHBYTES_" + to_string(cmds[i].size()) + " ";
                hex_str = bytes_to_hex_string(cmds[i].data(), cmds[i].size(), false);
                script_asm += hex_str;
                script_asm += " ";
                free(hex_str);
            }            
        }
    }
    while (script_asm.size() > 0 && script_asm[script_asm.size() - 1] == ' ') {
        script_asm.pop_back();
    }
    return script_asm;
}

Script::~Script() {}
