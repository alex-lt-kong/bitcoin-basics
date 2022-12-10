#include <inttypes.h>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "tx.h"
#include "utils.h"
#include "script.h"
#include "op.h"
#include "./cryptographic-algorithms/src/misc.h"
#include "./cryptographic-algorithms/src/sha256.h"


using namespace std;

void test_op_dup() {
  stack<vector<uint8_t>> data_stack;
  data_stack.push(vector<uint8_t>{'H','e','l','l','o',' ','w','o','r','l','d','\0'});
  op_dup(&data_stack);
  op_dup(&data_stack);
  cr_expect(data_stack.size() == 3);
  while (data_stack.empty() == false) {
    cr_expect(eq(str, (char*)data_stack.top().data(), "Hello world"));
    data_stack.pop();
  }
  bool ret_val = op_dup(&data_stack);
  cr_expect(ret_val == 0);
}

void test_op_hash256() {
  stack<vector<uint8_t>> data_stack;
  vector<uint8_t> hw_bytes = {'H','e','l','l','o',' ','w','o','r','l','d'};
  data_stack.push(hw_bytes);
  op_dup(&data_stack);
  op_hash256(&data_stack);
  cr_expect(data_stack.size() == 2);
  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false),
      "f6dc724d119649460e47ce719139e521e082be8a9755c5bece181de046ee65fe"
    )
  );
  op_hash256(&data_stack);
  cr_expect(data_stack.size() == 2);
  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false),
      "7a24c923419d8bd575e25f4f8e895d80648fb3ce2487de146e7a6ce5e7b6955d"
    )
  );
  op_hash256(&data_stack);
  cr_expect(data_stack.size() == 2);
  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false),
      "f1f37d379df9bf407af43757558c7cb48fdd16d58b95d16df74573bd97c8f316"
    )
  );
}

void test_op_hash160() {
  stack<vector<uint8_t>> data_stack;
  vector<uint8_t> hw_bytes = {'h','e','l','l','o',' ','w','o','r','l','d'};
  data_stack.push(hw_bytes);
  op_dup(&data_stack);
  op_hash160(&data_stack);
  cr_expect(data_stack.size() == 2);
  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), RIPEMD160_HASH_SIZE, false),
      "d7d5ee7824ff93f94c3055af9382c86c68b5ca92"
    )
  );

  op_dup(&data_stack);
  op_hash160(&data_stack);
  cr_expect(data_stack.size() == 3);

  cr_expect(
    eq(
      str, bytes_to_hex_string(data_stack.top().data(), RIPEMD160_HASH_SIZE, false),
      "f60b46dc1b792f72ba3b09b22cb48b592cc67b3b"
    )
  );
}


Test(ch06_test_suite, test_op_dup) {
  test_op_dup();
}


Test(ch06_test_suite, test_op_hash256) {
  test_op_hash256();
}

Test(ch06_test_suite, test_op_hash160) {
  test_op_hash160();
}

Test(ch06_test_suite, test_script_parsing_and_serialization1) {
  size_t input_len;
  
  // The input hex string is from the book
  char hex_chr[] = "6a47304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc26125022008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a7160121035d5c93d9ac96881f19ba1f686f15f009ded7c62efe85a872e6a19b43c15a2937";
  char* hex_input = (char*)hex_string_to_bytes(hex_chr, &input_len);
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_input, input_len);
  free(hex_input);
  Script my_script = Script();
  bool ret_val = my_script.parse(d);
  cr_expect(ret_val == 1);
  cr_expect(my_script.get_cmds().size() == 2);
  vector<vector<uint8_t>> cmds = my_script.get_cmds();
  cr_expect(
    eq(
      str, bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false),
      "304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc2612502"
      "2008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a71601"
    )
  );
  cr_expect(
    eq(
      str, bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false),
      "035d5c93d9ac96881f19ba1f686f15f009ded7c62efe85a872e6a19b43c15a2937"
    )
  ); 

  vector<uint8_t> out_bytes = my_script.serialize();
  cr_expect(eq(str, bytes_to_hex_string(out_bytes.data(), out_bytes.size(), false), hex_chr));
}

Test(ch06_test_suite, test_script_parsing_and_serialization2) {
  // This test case is manually constructed from transaction:
  // d21736be48b88d15591b101ecadcba8f65713876ffb2b29d60de01dfaef8b120
  size_t input_len;
  char hex_chr[] = "1976a914cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc088ac";
  uint8_t* hex_bytes = hex_string_to_bytes(hex_chr, &input_len);
  // 0x19 is 25, which is the length of 76a914cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc088ac encoded in varint.
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_bytes, input_len);
  free(hex_bytes);
  Script my_script = Script();
  bool ret_val = my_script.parse(d);
  cr_expect(ret_val == 1);
  cr_expect(my_script.get_cmds().size() == 5);
  vector<vector<uint8_t>> cmds = my_script.get_cmds();
  cr_expect(eq(str, bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false), "76"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false), "a9"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[2].data(), cmds[2].size(), false), "cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc0"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[3].data(), cmds[3].size(), false), "88"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[4].data(), cmds[4].size(), false), "ac"));

  vector<uint8_t> out_bytes = my_script.serialize();
  cr_expect(eq(str, bytes_to_hex_string(out_bytes.data(), out_bytes.size(), false), hex_chr));
}

Test(ch06_test_suite, test_script_parsing_and_serialization3_OP_PUSHDATA1) {
  // This test case is manually constructed from transaction:
  // d5ba15fecd9ef1cb27005a83380f6387dd30d7e940f341715c6d80cdbe1059ba
  size_t input_len;
  char hex_chr[] = "fd1f0100483045022100990eabdb96bbcdb961864c781a6868dee3d0dada85e0a4d818c61fe16e885d8c02200ae8744e2833b46194fbe5e54aa9b"
    "9bbc9c6f773d23dbf373446b975dac6efd6014730440220166ee94db70f2dbf654f388d2719433f0ebaf909f11cba1dad31a3e076ec8b19"
    "02204cf972f21b07403d3f8f1259ce4af0c81fd39d1425f09f2fbed97eaa125ccccf014c8b5221020561ef602a5d29c9aa1007772c1799b"
    "3802df9a1995ea7aae06d3cc81ba254ad21026b29b39135808c8480cf17b1e0b383588e9301f5c6242f77ca6276a4345f5c8e2102f8b708"
    "a979928d301cbb697217385a287f1dcae6f7f0b51321e58e0f8e04cc1c21032c6023144a138c31b3b5f4aec7a84d9b71a4c6ceaa7abe3f9"
    "311071407541be554ae";
  char* hex_input = (char*)hex_string_to_bytes(hex_chr, &input_len);
  // The first 3 bytes: 0xfd,0x1f,0x01 is 287,
  // which is the length of 76a914cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc088ac encoded in varint.
  
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_input, input_len);
  free(hex_input);
  Script my_script = Script();
  bool ret_val = my_script.parse(d);
  cr_expect(ret_val == 1);  
  cr_expect(my_script.get_cmds().size() == 4);
  vector<vector<uint8_t>> cmds = my_script.get_cmds();
  cr_expect(eq(str, bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false), "00"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false), "3045022100990eabdb96bbcdb961864c781a6868dee3d0dada85e0a4d818c61fe16e885d8c02200ae8744e2833b46194fbe5e54aa9b9bbc9c6f773d23dbf373446b975dac6efd601"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[2].data(), cmds[2].size(), false), "30440220166ee94db70f2dbf654f388d2719433f0ebaf909f11cba1dad31a3e076ec8b1902204cf972f21b07403d3f8f1259ce4af0c81fd39d1425f09f2fbed97eaa125ccccf01"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[3].data(), cmds[3].size(), false), "5221020561ef602a5d29c9aa1007772c1799b3802df9a1995ea7aae06d3cc81ba254ad21026b29b39135808c8480cf17b1e0b383588e9301f5c6242f77ca6276a4345f5c8e2102f8b708a979928d301cbb697217385a287f1dcae6f7f0b51321e58e0f8e04cc1c21032c6023144a138c31b3b5f4aec7a84d9b71a4c6ceaa7abe3f9311071407541be554ae"));

  vector<uint8_t> out_bytes = my_script.serialize();
  cr_expect(eq(str, bytes_to_hex_string(out_bytes.data(), out_bytes.size(), false), hex_chr));
}

Test(ch06_test_suite, test_script_parsing_and_serialization4_OP_PUSHDATA2) {
  // This test case is manually constructed from transaction:
  // d29c9c0e8e4d2a9790922af73f0b8d51f0bd4bb19940d9cf910ead8fbe85bc9b
  size_t input_len;
  char hex_chr[] = "fddb036a4dd7035765277265206e6f20737472616e6765727320746f206c6f76650a596f75206b6e6f77207468652072756c657320616e6420736"
    "f20646f20490a412066756c6c20636f6d6d69746d656e74277320776861742049276d207468696e6b696e67206f660a596f7520776f756c"
    "646e27742067657420746869732066726f6d20616e79206f74686572206775790a49206a7573742077616e6e612074656c6c20796f75206"
    "86f772049276d206665656c696e670a476f747461206d616b6520796f7520756e6465727374616e640a0a43484f5255530a4e6576657220"
    "676f6e6e61206769766520796f752075702c0a4e6576657220676f6e6e61206c657420796f7520646f776e0a4e6576657220676f6e6e612"
    "072756e2061726f756e6420616e642064657365727420796f750a4e6576657220676f6e6e61206d616b6520796f75206372792c0a4e6576"
    "657220676f6e6e612073617920676f6f646279650a4e6576657220676f6e6e612074656c6c2061206c696520616e64206875727420796f7"
    "50a0a5765277665206b6e6f776e2065616368206f7468657220666f7220736f206c6f6e670a596f75722068656172742773206265656e20"
    "616368696e672062757420796f7527726520746f6f2073687920746f207361792069740a496e7369646520776520626f7468206b6e6f772"
    "0776861742773206265656e20676f696e67206f6e0a5765206b6e6f77207468652067616d6520616e6420776527726520676f6e6e612070"
    "6c61792069740a416e6420696620796f752061736b206d6520686f772049276d206665656c696e670a446f6e27742074656c6c206d65207"
    "96f7527726520746f6f20626c696e6420746f20736565202843484f525553290a0a43484f52555343484f5255530a284f6f682067697665"
    "20796f75207570290a284f6f68206769766520796f75207570290a284f6f6829206e6576657220676f6e6e6120676976652c206e6576657"
    "220676f6e6e6120676976650a286769766520796f75207570290a284f6f6829206e6576657220676f6e6e6120676976652c206e65766572"
    "20676f6e6e6120676976650a286769766520796f75207570290a0a5765277665206b6e6f776e2065616368206f7468657220666f7220736"
    "f206c6f6e670a596f75722068656172742773206265656e20616368696e672062757420796f7527726520746f6f2073687920746f207361"
    "792069740a496e7369646520776520626f7468206b6e6f7720776861742773206265656e20676f696e67206f6e0a5765206b6e6f7720746"
    "8652067616d6520616e6420776527726520676f6e6e6120706c61792069742028544f2046524f4e54290a0a";
  char* hex_input = (char*)hex_string_to_bytes(hex_chr, &input_len);
  // The first 3 bytes: 0xfd,0xdb,0x03 is 987,
  // which is the length of hex_input encoded in varint.
  
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_input, input_len);
  free(hex_input);
  Script my_script = Script();
  bool ret_val = my_script.parse(d);
  cr_expect(ret_val == 1);  
  cr_expect(my_script.get_cmds().size() == 2);
  vector<vector<uint8_t>> cmds = my_script.get_cmds();
  cr_expect(eq(str, bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false), "6a"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false), "5765277265206e6f20737472616e6765727320746f206c6f76650a596f75206b6e6f77207468652072756c657320616e6420736f20646f20490a412066756c6c20636f6d6d69746d656e74277320776861742049276d207468696e6b696e67206f660a596f7520776f756c646e27742067657420746869732066726f6d20616e79206f74686572206775790a49206a7573742077616e6e612074656c6c20796f7520686f772049276d206665656c696e670a476f747461206d616b6520796f7520756e6465727374616e640a0a43484f5255530a4e6576657220676f6e6e61206769766520796f752075702c0a4e6576657220676f6e6e61206c657420796f7520646f776e0a4e6576657220676f6e6e612072756e2061726f756e6420616e642064657365727420796f750a4e6576657220676f6e6e61206d616b6520796f75206372792c0a4e6576657220676f6e6e612073617920676f6f646279650a4e6576657220676f6e6e612074656c6c2061206c696520616e64206875727420796f750a0a5765277665206b6e6f776e2065616368206f7468657220666f7220736f206c6f6e670a596f75722068656172742773206265656e20616368696e672062757420796f7527726520746f6f2073687920746f207361792069740a496e7369646520776520626f7468206b6e6f7720776861742773206265656e20676f696e67206f6e0a5765206b6e6f77207468652067616d6520616e6420776527726520676f6e6e6120706c61792069740a416e6420696620796f752061736b206d6520686f772049276d206665656c696e670a446f6e27742074656c6c206d6520796f7527726520746f6f20626c696e6420746f20736565202843484f525553290a0a43484f52555343484f5255530a284f6f68206769766520796f75207570290a284f6f68206769766520796f75207570290a284f6f6829206e6576657220676f6e6e6120676976652c206e6576657220676f6e6e6120676976650a286769766520796f75207570290a284f6f6829206e6576657220676f6e6e6120676976652c206e6576657220676f6e6e6120676976650a286769766520796f75207570290a0a5765277665206b6e6f776e2065616368206f7468657220666f7220736f206c6f6e670a596f75722068656172742773206265656e20616368696e672062757420796f7527726520746f6f2073687920746f207361792069740a496e7369646520776520626f7468206b6e6f7720776861742773206265656e20676f696e67206f6e0a5765206b6e6f77207468652067616d6520616e6420776527726520676f6e6e6120706c61792069742028544f2046524f4e54290a0a"));

  vector<uint8_t> out_bytes = my_script.serialize();
  cr_expect(eq(str, bytes_to_hex_string(out_bytes.data(), out_bytes.size(), false), hex_chr));
}

Test(ch06_test_suite, test_script_parsing_and_serialization5_OP_PUSH) {
  // This test case is manually constructed from transaction:
  // 23b898cadf9ce45123dbdb1ab7e676d96eaf672297801a013b26f349892e1bb5
  size_t input_len;
  char hex_chr[] = "055556935b87";
  char* hex_input = (char*)hex_string_to_bytes(hex_chr, &input_len);
  
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_input, input_len);
  free(hex_input);
  Script my_script = Script();
  bool ret_val = my_script.parse(d);
  cr_expect(ret_val == 1);
  cr_expect(my_script.get_cmds().size() == 5);
  vector<vector<uint8_t>> cmds = my_script.get_cmds();
  cr_expect(eq(str, bytes_to_hex_string(cmds[0].data(), cmds[0].size(), false), "55"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[1].data(), cmds[1].size(), false), "56"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[2].data(), cmds[2].size(), false), "93"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[3].data(), cmds[3].size(), false), "5b"));
  cr_expect(eq(str, bytes_to_hex_string(cmds[4].data(), cmds[4].size(), false), "87"));

  vector<uint8_t> out_bytes = my_script.serialize();
  cr_expect(eq(str, bytes_to_hex_string(out_bytes.data(), out_bytes.size(), false), hex_chr));
}