#include <inttypes.h>
#include <mycrypto/misc.hpp>
#include <stdio.h>
#include <stdlib.h>

#include "mybitcoin/script.h"
#include "mybitcoin/tx.h"
#include "mybitcoin/utils.h"
#include "script-test.h"

using namespace std;

int main(int argc, char **argv) {
  if (argc != 3 && argc != 2) {
    cerr << "Usage " << argv[0] << " <script hex> <script asm>" << endl;
    cerr << "Usage " << argv[0] << " <testcase path>" << endl;
    return 1;
  }
  int64_t input_bytes_len;
  size_t varint_len;

  string script_hex = argv[1];
  string script_asm = argv[2];

  unique_fptr<uint8_t[]> input_bytes(
      hex_string_to_bytes(script_hex.c_str(), &input_bytes_len));
  if (input_bytes.get() == NULL) {
    fprintf(stderr, "invalid script_hex: %s\n", script_hex.c_str());
    return 1;
  }

  vector<uint8_t> d(input_bytes_len);
  memcpy(d.data(), input_bytes.get(), input_bytes_len);
  uint8_t *input_len_varint = encode_variable_int(input_bytes_len, &varint_len);
  for (int i = varint_len - 1; i >= 0; --i) {
    d.insert(d.begin(), input_len_varint[i]);
  }
  free(input_len_varint);

  Script my_script = Script(d);
  size_t cmds_size = my_script.get_cmds().size();
  if (cmds_size < 1) {
    fprintf(stderr, "unexpected cmds_size: %lu\n", cmds_size);
    return 1;
  }

  vector<vector<uint8_t>> cmds = my_script.get_cmds();
  vector<bool> is_opcode = my_script.get_is_opcode();
  if (is_opcode.size() != cmds.size()) {
    fprintf(stderr, "is_opcode.size() != cmds.size()\n");
    return 1;
  }

  int ret_val = 0;

  vector<uint8_t> out_d = my_script.serialize();
  if (out_d.size() == 0) {
    printf("failed to serialize() Script\n");
    return 1;
  }
  char *serialized_chrs = bytes_to_hex_string(out_d.data() + varint_len,
                                              out_d.size() - varint_len, false);

  if (strcmp(serialized_chrs, script_hex.c_str()) != 0) {
    fprintf(stderr,
            "parse() and serialize() result in different byte "
            "string:\nActual: %s\nExpect: %s\n",
            serialized_chrs, script_hex.c_str());
    ++ret_val;
  }
  free(serialized_chrs);

  if (exception_dict.find(script_hex.c_str()) != exception_dict.end() && 0) {
    if (strcmp(my_script.get_asm().c_str(),
               exception_dict.find(script_hex.c_str())->second.c_str()) != 0) {
      fprintf(stderr,
              "get_asm() and exception_dict()->second are different:\nActual: "
              "%s\nExpect: %s\n",
              my_script.get_asm().c_str(),
              exception_dict.find(script_hex.c_str())->second.c_str());
      ++ret_val;
    }
  } else {
    if (strcmp(my_script.get_asm().c_str(), script_asm.c_str()) != 0) {
      fprintf(
          stderr,
          "get_asm() and script_asm are different:\nActual: %s\nExpect: %s\n",
          my_script.get_asm().c_str(), script_asm.c_str());
      ++ret_val;
    }
  }
  if (ret_val == 0) {
    printf("okay\n");
  } else {
    printf("error\n");
  }

  cmds = my_script.get_cmds();
  return ret_val;
}
