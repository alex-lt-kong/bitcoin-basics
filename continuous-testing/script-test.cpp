#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/tx.h"
#include "../src/script.h"
#include "../src/utils.h"
#include "../cryptographic-algorithms/src/misc.h"

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

  size_t space_count = 0;
  for (size_t i = 0; i < strnlen(argv[2], PATH_MAX); ++i) {
    if (argv[2][i] == ' ') {
      ++space_count;
    }
  }
  for (size_t i = 0; i < cmds.size(); ++i) {
    if (is_opcode[i] == false) {
      --space_count;
    }
  }
  if (space_count + 1 != cmds.size()) {
    if (strstr(argv[2], "<push past end>") == NULL) {
      fprintf(stderr, "space_count + 1 != cmds.size()\n (%lu vs %lu)\n", space_count, cmds.size());
      return 1;
    } else {
      if (space_count != cmds.size() + 1) {
        fprintf(stderr, "space_count != cmds.size() + 1 (%lu vs %lu)\n", space_count, cmds.size());
        return 1;
      }
    }     
  }
  //printf("%s\n", my_script.get_asm().c_str());
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
  } else {
    fprintf(
      stderr,
      "the Script is non-standard, the serialize() check will be skipped and "
      "the serialize()'ed bytes array is very likely to be different\n"
    );
  }
  
  printf("okay\n");
  return 0;
}
