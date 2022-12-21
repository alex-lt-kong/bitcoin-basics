#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <mycrypto/misc.h>

#include "../src/tx.h"
#include "../src/script.h"
#include "../src/utils.h"


using namespace std;


int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage script-test.out <script hex> <script asm>\n");
    return 1;
  }
  size_t input_bytes_len, varint_len;
  uint8_t* input_bytes = hex_string_to_bytes(argv[1], &input_bytes_len); 

  vector<uint8_t> d(input_bytes_len);
  memcpy(d.data(), input_bytes, input_bytes_len);
  uint8_t* input_len_varint = encode_variable_int(input_bytes_len, &varint_len);
  free(input_bytes);
  for (int i = varint_len - 1; i >= 0; --i) {
    d.insert(d.begin(), input_len_varint[i]);
  }
  free(input_len_varint);
  
  Script my_script = Script();
  bool retval = my_script.parse(d);  
  if (retval != true) {
    fprintf(stderr, "parse() failed\n");
    return 1;
  }
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
  
  if (!my_script.is_nonstandard_script_parsed()) {

    vector<uint8_t> out_d = my_script.serialize();
    if (out_d.size() == 0) {
      printf("failed to serialize() Script\n");
      return 1;
    }
    char* serialized_chrs = bytes_to_hex_string(out_d.data() + varint_len, out_d.size() - varint_len, false);

    if (strcmp(serialized_chrs, argv[1]) != 0) {
      fprintf(stderr, "parse() and serialize() result in different byte string:\nActual: %s\nExpect:%s\n", serialized_chrs, argv[1]);
    }
    free(serialized_chrs);
    if (strcmp(my_script.get_asm().c_str(), argv[2]) != 0) {
      fprintf(stderr, "get_asm() and script_asm are different:\nActual: %s\nExpect: %s\n", my_script.get_asm().c_str(), argv[2]);
      ++ret_val;
    }
  } else {
    fprintf(
      stderr,
      "the Script is non-standard, the serialize() check will be skipped and "
      "the serialize()'ed bytes array is very likely to be different\n"
    );
  }
  if (ret_val == 0) {
    printf("okay\n");
  } else {
    printf("error\n");
  }  
  return ret_val;
}
