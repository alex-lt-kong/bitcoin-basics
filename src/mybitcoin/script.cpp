#include <mycrypto/misc.hpp>

#include "script.h"
#include "utils.h"

Script::Script() {}

Script::Script(vector<uint8_t>& d) {
    // https://en.bitcoin.it/wiki/Script
    uint64_t remaining_script_len = read_variable_int(d);
    uint8_t cb = 0; // current byte
    cmds.clear();
    is_opcode.clear();
    const char cmd_names[][13] = {"OP_PUSHDATA1", "OP_PUSHDATA2",
        "OP_PUSHDATA4"};
    while (remaining_script_len > 0) {
        if (d.size() == 0) {
            throw invalid_argument("byte_stream ends unexpectedly");
        }
        cb = d[0];
        d.erase(d.begin());
        -- remaining_script_len;
        if (cb >= 1 && cb <= 75) {
            // an ordinary element should be between 1 to 75 bytes. If cb is
            // smaller then 75, it implies this component
            // is an ordinary element (i.e., an operand, not an opcode)
            uint8_t nominal_len = cb;
            uint8_t actual_len = cb;
            if (actual_len > remaining_script_len) {
                // Will only enter this branch if the current operand is the last one.
                fprintf(stderr, "Non-standard Script: push past end\n");
                actual_len = remaining_script_len;
            }
            last_operand = vector<uint8_t>(1 + actual_len);
            last_operand[0] = nominal_len;
            vector<uint8_t> cmd(actual_len);
            memcpy(cmd.data(), d.data(), actual_len);
            memcpy(last_operand.data() + 1 * sizeof(uint8_t),
                   d.data(), actual_len);
            d.erase(d.begin(), d.begin() + actual_len);
            
            /*
                What if actual_len == 0?
                According to: https://en.cppreference.com/w/cpp/container/vector/erase
                If last == end() prior to removal, then the updated end() iterator is returned.
                If [first, last) is an empty range, then last is returned. 
                end() returns an iterator referring to the past-the-end element in the vector container. The
                past-the-end element is the theoretical element that would follow the last element in the vector.
                It does not point to any element, and thus shall not be dereferenced. 
                Therefore, it seems that actual_len == 0 and d.begin() == end() are both valid.
            */
            cmds.push_back(cmd);
            is_opcode.push_back(false);
            remaining_script_len -= actual_len; // curr_byte stores the size of the operand
        } else if (cb >= 76 && cb <= 78) {
            // 76,77,78 corresponds to OP_PUSHDATA1/OP_PUSHDATA2/OP_PUSHDATA4,
            // meaning that we read the next 1,2,4 bytes
            // which, in little endian order, specify how many bytes the element has.
            cmds.push_back(vector<uint8_t>{ cb });
            is_opcode.push_back(true);
            size_t OP_PUSHDATA_size = (
                remaining_script_len > 
                get_nominal_operand_len_byte_count_after_op_pushdata(cb) ?
                get_nominal_operand_len_byte_count_after_op_pushdata(cb) :
                remaining_script_len
            );
            if (OP_PUSHDATA_size < 
                get_nominal_operand_len_byte_count_after_op_pushdata(cb)) {
                // Will only enter this branch if the coming operand is the last one.
                // It also implies that OP_PUSHDATA pushes nothing at all!
                // (as it has already reached the end of d.)
                fprintf(stderr, "Non-standard Script: %lu too short for %s and "
                        "it pushes no data at all\n", remaining_script_len,
                        cmd_names[cb-76]);
            }

            size_t actual_operand_len = get_nominal_operand_len_after_op_pushdata(cb, d);            
            if (actual_operand_len > remaining_script_len - OP_PUSHDATA_size) {
                // Though not explicitly put in if, program will only enter this
                // branch if the coming operand is the last one.
                fprintf(stderr, "Non-standard Script: push past end\n");
                actual_operand_len = remaining_script_len - OP_PUSHDATA_size;
            }

            if (actual_operand_len > 520) {
                cerr << "Non-standard Script: actual_operand_len > 520" << endl;
                if (actual_operand_len > 4096) {
                    throw invalid_argument("Non-standard Script: "
                    "actual_operand_len > 4096");
                }
            }
            last_operand = vector<uint8_t>(OP_PUSHDATA_size + actual_operand_len);
            if (OP_PUSHDATA_size > 0) {
                memcpy(last_operand.data(), d.data(), OP_PUSHDATA_size);
                d.erase(d.begin(), d.begin() + OP_PUSHDATA_size);
            }
            vector<uint8_t> cmd(actual_operand_len);
            if (actual_operand_len > 0) {
                memcpy(cmd.data(), d.data(), actual_operand_len);
                memcpy(last_operand.data() + OP_PUSHDATA_size * sizeof(uint8_t),
                    d.data(), actual_operand_len);
                d.erase(d.begin(), d.begin() + actual_operand_len);
            }
            cmds.push_back(cmd);
            is_opcode.push_back(false);
            
            remaining_script_len -= last_operand.size();
        } else {
            // otherwise it is an opcode
            vector<uint8_t> cmd{ cb };
            cmds.push_back(cmd);
            is_opcode.push_back(true);
        }
    }
}

vector<uint8_t> Script::serialize() {
    vector<uint8_t> d(0);
    size_t idx = 0;
    while (idx < cmds.size()) {
        if (is_opcode[idx] != true) {
            size_t operand_len = 0;
            if (cmds.size() - 1 == idx) {
                operand_len = last_operand[0];
            } else {
                operand_len = cmds[idx].size();
            }
            if (operand_len >= 75) {
                cout << "Non-standard Script: operand longer than "
                     << "75 bytes without OP_PUSHDATA" << endl;
            }
            d.push_back(operand_len);
            for (size_t j = 0; j < cmds[idx].size(); ++j) {
                d.push_back(cmds[idx][j]);
            }
            ++idx;
            continue;
        }
        // is_opcode == true:
        if (cmds[idx].size() != 1) {
            throw invalid_argument("Opcode occupies more "
                "than one byte, which is supposed to be impossible");
        }

        if (cmds[idx][0] > 78 || cmds[idx][0] == 0) {
            d.push_back(cmds[idx][0]);
        } else if (cmds[idx][0] >= 76 && cmds[idx][0] <= 78) {
            d.push_back(cmds[idx][0]);

            ++idx;
            size_t operand_len = 0;
            if (idx != cmds.size() - 1) {
                operand_len = cmds[idx].size();                    
                d.push_back((uint8_t)operand_len);                 // for 0x0A0B0C0D, it extracts 0D at 0
                if (this->cmds[idx-1][0] >= 77) {
                    d.push_back((uint8_t)(operand_len >> 8));      // for 0x0A0B0C0D, it extracts 0C at 1
                    if (this->cmds[idx-1][0] >= 78) {
                        d.push_back((uint8_t)(operand_len >> 16)); // for 0x0A0B0C0D, it extracts 0B at 2
                        d.push_back((uint8_t)(operand_len >> 24)); // for 0x0A0B0C0D, it extracts 0A at 3
                    }
                }
                if (is_opcode[idx] == true) {
                    /* Use to handle this scenario:
                        ASM:      OP_BOOLOR OP_PUSHDATA1    OP_0 OP_CODESEPARATOR
                        Actual:          9b           4c 01   00               ab
                        Expect:          9b           4c 00   00               ab
                        */
                    operand_len = 0;
                }
            } else if (idx >= cmds.size()) {
                fprintf(stderr, "access violation, this is FATAL.\n");
                return vector<uint8_t>(0);
            } else {
                operand_len = last_operand.size();
                if (operand_len > 0) {
                    d.push_back(last_operand[0]);
                    if (cmds[idx-1][0] >= 77 && (operand_len > 1)) {
                        d.push_back((last_operand[1]));
                        if (cmds[idx-1][0] >= 78 && (operand_len > 2)) {
                            d.push_back((uint8_t)(last_operand[2]));
                            if (operand_len > 3) {
                                d.push_back((uint8_t)(last_operand[3]));
                            }
                        }
                    }
                }
            }
            if (operand_len != cmds[idx].size() +
                get_nominal_operand_len_byte_count_after_op_pushdata(
                    cmds[idx-1][0])) {
                cerr << "Non-standard Script: operand_len (" << operand_len
                     << ") is different from operand.size() + bytes used to "
                    "store operand_len ("
                     << cmds[idx].size() << "+"
                     << get_nominal_operand_len_byte_count_after_op_pushdata(
                         cmds[idx-1][0])
                     << ")" << endl;
            }
            for (size_t j = 0; j < operand_len && j < cmds[idx].size(); ++j) {
                d.push_back(cmds[idx][j]);
            }
            if (is_opcode[idx] == true) {
                /*
                It is a bit difficult to explain the case succinctly,
                it is needed to serialize() the following test case:
                "03cf760b1b4d696e656420627920416e74506f6f6c383738be00010045bd3903fabe6d6d8dee9c6ded1bbc251c0bdf56784cd8e32dc6c6448186a124ffdda854c54ff1eb02000000000000009b4c0000abc8000000000000"
                "OP_PUSHBYTES_3 cf760b OP_PUSHBYTES_27 4d696e656420627920416e74506f6f6c383738be00010045bd3903 OP_RETURN_250 OP_RETURN_190 OP_2DROP OP_2DROP OP_2MUL OP_RETURN_238 OP_NUMEQUAL OP_2DROP OP_RETURN_237 OP_PUSHBYTES_27 bc251c0bdf56784cd8e32dc6c6448186a124ffdda854c54ff1eb02 OP_0 OP_0 OP_0 OP_0 OP_0 OP_0 OP_0 OP_BOOLOR OP_PUSHDATA1 OP_0 OP_CODESEPARATOR OP_RETURN_200 OP_0 OP_0 OP_0 OP_0 OP_0 OP_0"
                */
                --idx;
            }
        } else {
            throw invalid_argument("Invalid opcode: " +
                to_string(cmds[idx][0]));
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

vector<vector<uint8_t>> Script::get_cmds() {
    return cmds;
}

vector<bool> Script::get_is_opcode() {
    return is_opcode;
}

size_t Script::get_nominal_operand_len_byte_count_after_op_pushdata(
    uint8_t opcode) {
    const size_t operand_lengths[] = {1, 2, 4};
    if (opcode < 76 || opcode > 78) {
        throw invalid_argument("Invalid opcode: " + to_string(opcode));
    }
    return operand_lengths[opcode - 76];
}

uint64_t Script::get_nominal_operand_len_after_op_pushdata(uint8_t opcode,
    vector<uint8_t> byte_stream) {
    
    size_t nominal_operand_len_byte_count = 
        get_nominal_operand_len_byte_count_after_op_pushdata(opcode);
    uint8_t buf[4] = {0};
    if (nominal_operand_len_byte_count > byte_stream.size()) {
        nominal_operand_len_byte_count = byte_stream.size();
        cerr << "Non-standard Script: push past end" << endl;
    }
    if (nominal_operand_len_byte_count > 0) {
        memcpy(buf, byte_stream.data(), nominal_operand_len_byte_count);
    }
    
    return buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
}

string Script::get_asm() {
    string script_asm = "";
    if (cmds.size() == 0) {
        fprintf(stderr, "cmds is empty, get_asm() is not "
                "supposed to be called!\n");
    }
    for (size_t i = 0; i < cmds.size(); ++i) {
        if (!is_opcode[i]) {
            if (cmds[i].size() > 75) {
                fprintf(stderr, "This should never happen\n");
                return "";
            }
            if (i == cmds.size() - 1 && cmds[i].size() != last_operand[0]) {
                script_asm += "OP_PUSHBYTES_" + to_string(last_operand[0]) + " ";
                script_asm += "<push past end>";
            } else {
                script_asm += "OP_PUSHBYTES_" + to_string(cmds[i].size()) + " ";
                unique_fptr<char[]>  hex_str(bytes_to_hex_string(cmds[i].data(),
                    cmds[i].size(), false));
                script_asm += hex_str.get();
                script_asm += " ";
            }
            continue;
        } 
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
                fprintf(stderr, "Non-standard Script: OP_PUSHDATA followed by "
                        "no data\n");
                continue;
            }
            ++i;
            if ((cmds[i].size() > 255 && cmds[i-1][0] == 76) ||
                (cmds[i].size() > 520 && cmds[i-1][0] >= 77)) {
                fprintf(stderr, "Non-standard Script: operand loaded by "
                        "OP_PUSHDATA has %lu bytes.\n", cmds[i].size());
            }
            if (i == cmds.size() - 1) {
                if (last_operand.size() < 
                    get_nominal_operand_len_byte_count_after_op_pushdata(
                        cmds[i-1][0])) {
                    // This is how blockstream.info handles this case:
                    // we need to dial back the "OP_PUSHDATA" opcode to match 
                    // its.
                    script_asm = script_asm.substr(
                        0, script_asm.size() - strlen("OP_PUSHDATA_ ")
                    );
                    if (script_asm.size() > 0 &&
                        script_asm[script_asm.size() - 1] == ' ') {
                        script_asm.pop_back();
                    }

                    script_asm += "<unexpected end>";
                    continue;
                } else if (get_nominal_operand_len_after_op_pushdata(
                    cmds[i-1][0], last_operand) > cmds[i].size()) {
                    script_asm += "<push past end>"; 
                    continue;
                }
            }
            if (cmds[i].size() > 0) {
                unique_fptr<char[]>  hex_str(bytes_to_hex_string(cmds[i].data(),
                    cmds[i].size(), false));
                script_asm += hex_str.get();
                script_asm += " ";
            }
        }
    }
    while (script_asm.size() > 0 && script_asm[script_asm.size() - 1] == ' ') {
        script_asm.pop_back();
    }
    return script_asm;
}

Script::~Script() {}
