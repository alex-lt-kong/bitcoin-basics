#include <inttypes.h>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <mycrypto/misc.h>
#include <mycrypto/sha256.h>

#include "mybitcoin/tx.h"
#include "mybitcoin/utils.h"
#include "mybitcoin/script.h"
#include "mybitcoin/op.h"


using namespace std;


int test_parsing_and_serialization_and_get_asm(const char* hex_str_in, const size_t expected_cmds_size, char expected_str_outs[][2048], const char* expected_asm) {
    int64_t input_len;
    
    char* hex_input = (char*)hex_string_to_bytes(hex_str_in, &input_len);
    
    vector<uint8_t> d(input_len);
    memcpy(d.data(), hex_input, input_len);
    free(hex_input);
    Script my_script = Script();
    bool ret_val = my_script.parse(d);
    if (ret_val != 1) {
        fprintf(stderr, "parse() failed\n");
        return 1;
    }
    if (my_script.get_cmds().size() != expected_cmds_size) {
        fprintf(stderr, "get_cmds().size():\nActual: %lu\nExpect: %lu\n", my_script.get_cmds().size(), expected_cmds_size);
        return 1;
    }
    vector<vector<uint8_t>> cmds = my_script.get_cmds();
    
    char* hex_str_out;
    for (size_t i = 0; i < expected_cmds_size; ++i) {
        hex_str_out = bytes_to_hex_string(cmds[i].data(), cmds[i].size(), false);
        if (strcmp(hex_str_out, expected_str_outs[i]) != 0) {
            fprintf(stderr, "get_cmds()[%lu]:\nActual: %s\nExpect: %s\n", i, hex_str_out, expected_str_outs[i]);
            free(hex_str_out);
            return 1;
        }
        free(hex_str_out);
    }

    vector<uint8_t> out_bytes = my_script.serialize();
    hex_str_out = bytes_to_hex_string(out_bytes.data(), out_bytes.size(), false);
    if (strcmp(hex_str_out, hex_str_in) != 0) {
        fprintf(stderr, "serialize():\nActual: %s\nExpect: %s\n", hex_str_out, hex_str_in);
        free(hex_str_out);
        return 1;
    }
    free(hex_str_out);

    if (expected_asm != NULL) {
        string actual_asm = my_script.get_asm();
        if (strcmp(actual_asm.c_str(), expected_asm) != 0) {
            fprintf(stderr, "get_asm():\nExpect: %s\nActual: %s\n", expected_asm, actual_asm.c_str());
            return 1;
        }
        // free(actual_asm); Don't free() it! It belongs to my_script!
    }
    return 0;
}

int test_op_dup() {
    stack<vector<uint8_t>> data_stack;
    data_stack.push(vector<uint8_t>{'H','e','l','l','o',' ','w','o','r','l','d','\0'});
    get_opcode(118).func_ptr(data_stack);
    get_opcode(118).func_ptr(data_stack);
    if (data_stack.size() != 3) {
        return 1;
    }
    while (data_stack.empty() == false) {
        if (strcmp((char*)data_stack.top().data(), "Hello world") != 0) {
            return 1;
        }
        data_stack.pop();
    }
    bool ret_val = get_opcode(118).func_ptr(data_stack);
    if (ret_val != 0) {
        return 1;
    }
    return 0;
}

int test_op_hash256() {
    int retval = 0;
    char* hex_chr = NULL;
    stack<vector<uint8_t>> data_stack;
    vector<uint8_t> hw_bytes = {'H','e','l','l','o',' ','w','o','r','l','d'};
    data_stack.push(hw_bytes);
    get_opcode(118).func_ptr(data_stack);
    get_opcode(170).func_ptr(data_stack);
    if (data_stack.size() != 2) {
        return 1;
    }
    hex_chr = bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false);
    if (strcmp(hex_chr, "f6dc724d119649460e47ce719139e521e082be8a9755c5bece181de046ee65fe") != 0) {
        retval = 1;
        goto finally;
    }
    free(hex_chr);
    get_opcode(0xaa).func_ptr(data_stack);
    if (data_stack.size() != 2) {
        retval = 1;
        goto finally;
    }
    hex_chr = bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false);
    if (strcmp(hex_chr, "7a24c923419d8bd575e25f4f8e895d80648fb3ce2487de146e7a6ce5e7b6955d") != 0) {
        retval = 1;
        goto finally;
    }
    
    free(hex_chr);
    get_opcode(170).func_ptr(data_stack);
    if (data_stack.size() != 2) {
        retval = 1;
        goto finally;
    }
    hex_chr = bytes_to_hex_string(data_stack.top().data(), SHA256_HASH_SIZE, false);
    if (strcmp(hex_chr, "f1f37d379df9bf407af43757558c7cb48fdd16d58b95d16df74573bd97c8f316") != 0) {
        retval = 1;
        goto finally;        
    }

finally:
    if (hex_chr != NULL) {
        free(hex_chr);
    }
    return retval;
}


int test_op_hash160() {
    int retval = 0;
    stack<vector<uint8_t>> data_stack;
    vector<uint8_t> hw_bytes = {'h','e','l','l','o',' ','w','o','r','l','d'};
    data_stack.push(hw_bytes);
    get_opcode(118).func_ptr(data_stack);
    get_opcode(169).func_ptr(data_stack);
    if (data_stack.size() != 2) {
        return 1;
    }
    char* hex_chr = NULL;
    hex_chr = bytes_to_hex_string(data_stack.top().data(), RIPEMD160_HASH_SIZE, false);
    if (strcmp(hex_chr, "d7d5ee7824ff93f94c3055af9382c86c68b5ca92") != 0) {
        retval = 1;
        goto finally;
    }
    free(hex_chr);
    hex_chr = NULL;
    get_opcode(118).func_ptr(data_stack);
    get_opcode(169).func_ptr(data_stack);
    if (data_stack.size() != 3) {
        return 1;
    }
    hex_chr = bytes_to_hex_string(data_stack.top().data(), RIPEMD160_HASH_SIZE, false);
    if (strcmp(hex_chr, "f60b46dc1b792f72ba3b09b22cb48b592cc67b3b") != 0) {
        retval = 1;
        goto finally;
    }
finally:
    if (hex_chr != NULL) {
        free(hex_chr);
    }
    return retval;
}

int test_script_parsing_and_serialization1() {        
    // The input hex string is from Jimmy's book
    char hex_str_in[] = "6a47304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc26125022008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a7160121035d5c93d9ac96881f19ba1f686f15f009ded7c62efe85a872e6a19b43c15a2937";
    const size_t expected_cmds_size = 2;
    char expected_str_outs[expected_cmds_size][2048] = {
        "304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc26125022008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a71601",
        "035d5c93d9ac96881f19ba1f686f15f009ded7c62efe85a872e6a19b43c15a2937"
    };
    return test_parsing_and_serialization_and_get_asm(hex_str_in, expected_cmds_size, expected_str_outs, NULL);
}


int test_script_parsing_and_serialization2() {
    // This test case is manually constructed from transaction:
    // d21736be48b88d15591b101ecadcba8f65713876ffb2b29d60de01dfaef8b120
    char hex_str_in[] = "1976a914cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc088ac";
    const size_t expected_cmds_size = 5;
    char expected_str_outs[expected_cmds_size][2048] = {
        "76", "a9", "cebb2851a9c7cfe2582c12ecaf7f3ff4383d1dc0", "88", "ac"
    };
    return test_parsing_and_serialization_and_get_asm(hex_str_in, expected_cmds_size, expected_str_outs, NULL);
}

int test_script_parsing_and_serialization3_OP_PUSHDATA1() {
    // This test case is manually constructed from transaction:
    // d5ba15fecd9ef1cb27005a83380f6387dd30d7e940f341715c6d80cdbe1059ba
    char hex_str_in[] = "fd1f0100483045022100990eabdb96bbcdb961864c781a6868dee3d0dada85e0a4d818c61fe16e885d8c02200ae8744e2833b46194fbe5e54aa9b"
        "9bbc9c6f773d23dbf373446b975dac6efd6014730440220166ee94db70f2dbf654f388d2719433f0ebaf909f11cba1dad31a3e076ec8b19"
        "02204cf972f21b07403d3f8f1259ce4af0c81fd39d1425f09f2fbed97eaa125ccccf014c8b5221020561ef602a5d29c9aa1007772c1799b"
        "3802df9a1995ea7aae06d3cc81ba254ad21026b29b39135808c8480cf17b1e0b383588e9301f5c6242f77ca6276a4345f5c8e2102f8b708"
        "a979928d301cbb697217385a287f1dcae6f7f0b51321e58e0f8e04cc1c21032c6023144a138c31b3b5f4aec7a84d9b71a4c6ceaa7abe3f9"
        "311071407541be554ae";
    const size_t expected_cmds_size = 5;
    char expected_str_outs[expected_cmds_size][2048] = {
        "00",
        "3045022100990eabdb96bbcdb961864c781a6868dee3d0dada85e0a4d818c61fe16e885d8c02200ae8744e2833b46194fbe5e54aa9b9bbc9c6f773d23dbf373446b975dac6efd601",
        "30440220166ee94db70f2dbf654f388d2719433f0ebaf909f11cba1dad31a3e076ec8b1902204cf972f21b07403d3f8f1259ce4af0c81fd39d1425f09f2fbed97eaa125ccccf01",
        "4c",
        "5221020561ef602a5d29c9aa1007772c1799b3802df9a1995ea7aae06d3cc81ba254ad21026b29b39135808c8480cf17b1e0b383588e9301f5c6242f77ca6276a4345f5c8e2102f8b708a979928d301cbb697217385a287f1dcae6f7f0b51321e58e0f8e04cc1c21032c6023144a138c31b3b5f4aec7a84d9b71a4c6ceaa7abe3f9311071407541be554ae"
    };
    return test_parsing_and_serialization_and_get_asm(hex_str_in, expected_cmds_size, expected_str_outs, NULL);
}

int test_script_parsing_and_serialization4_OP_PUSHDATA2() {
    // This test case is manually constructed from transaction:
    // d29c9c0e8e4d2a9790922af73f0b8d51f0bd4bb19940d9cf910ead8fbe85bc9b

    char hex_str_in[] = "fddb036a4dd7035765277265206e6f20737472616e6765727320746f206c6f76650a596f75206b6e6f77207468652072756c657320616e6420736"
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
    const size_t expected_cmds_size = 3;
    char expected_str_outs[expected_cmds_size][2048] = {
        "6a",
        "4d",
        "5765277265206e6f20737472616e6765727320746f206c6f76650a596f75206b6e6f77207468652072756c657320616e6420736f20646f20490a412066756c6c20636f6d6d69746d656e74277320776861742049276d207468696e6b696e67206f660a596f7520776f756c646e27742067657420746869732066726f6d20616e79206f74686572206775790a49206a7573742077616e6e612074656c6c20796f7520686f772049276d206665656c696e670a476f747461206d616b6520796f7520756e6465727374616e640a0a43484f5255530a4e6576657220676f6e6e61206769766520796f752075702c0a4e6576657220676f6e6e61206c657420796f7520646f776e0a4e6576657220676f6e6e612072756e2061726f756e6420616e642064657365727420796f750a4e6576657220676f6e6e61206d616b6520796f75206372792c0a4e6576657220676f6e6e612073617920676f6f646279650a4e6576657220676f6e6e612074656c6c2061206c696520616e64206875727420796f750a0a5765277665206b6e6f776e2065616368206f7468657220666f7220736f206c6f6e670a596f75722068656172742773206265656e20616368696e672062757420796f7527726520746f6f2073687920746f207361792069740a496e7369646520776520626f7468206b6e6f7720776861742773206265656e20676f696e67206f6e0a5765206b6e6f77207468652067616d6520616e6420776527726520676f6e6e6120706c61792069740a416e6420696620796f752061736b206d6520686f772049276d206665656c696e670a446f6e27742074656c6c206d6520796f7527726520746f6f20626c696e6420746f20736565202843484f525553290a0a43484f52555343484f5255530a284f6f68206769766520796f75207570290a284f6f68206769766520796f75207570290a284f6f6829206e6576657220676f6e6e6120676976652c206e6576657220676f6e6e6120676976650a286769766520796f75207570290a284f6f6829206e6576657220676f6e6e6120676976652c206e6576657220676f6e6e6120676976650a286769766520796f75207570290a0a5765277665206b6e6f776e2065616368206f7468657220666f7220736f206c6f6e670a596f75722068656172742773206265656e20616368696e672062757420796f7527726520746f6f2073687920746f207361792069740a496e7369646520776520626f7468206b6e6f7720776861742773206265656e20676f696e67206f6e0a5765206b6e6f77207468652067616d6520616e6420776527726520676f6e6e6120706c61792069742028544f2046524f4e54290a0a"
    };
    return test_parsing_and_serialization_and_get_asm(hex_str_in, expected_cmds_size, expected_str_outs, NULL);
}

int test_script_parsing_and_serialization5_OP_PUSH() {
    // This test case is manually constructed from transaction:
    // 23b898cadf9ce45123dbdb1ab7e676d96eaf672297801a013b26f349892e1bb5
    char hex_str_in[] = "055556935b87";
    const size_t expected_cmds_size = 5;
    char expected_str_outs[expected_cmds_size][2048] = {"55", "56", "93", "5b", "87"};
    return test_parsing_and_serialization_and_get_asm(hex_str_in, expected_cmds_size, expected_str_outs, NULL);
}

int test_script_parsing_and_serialization6_special_cases() {
    printf("\n=== A lot of warnings are expected but none of them should break "
           "the program===\n");
             
    const size_t buf_size = 2048;
    // If hex string is copied from blockstream.info, need to manually append 
    // the varint-encoded length of the script to it. (e.g., 41 and 2c)
    // at the beginning of the first two strings.
    char hex_str_in[][buf_size] = {
        "41fc70035c7a81bc6fcc36947f7c1b2d63620560bec2aa336a676213bab74c9f03d46788100dca84c0f19a0f1c14ef0d67f3fc63c011ba4787510d55fde9554e554e",
        "2c6a4c2952534b424c4f434b3a3f6536dbb51cbe76519e0cd70480cf7f07da4ae2334cac402ef18329004681c4",
        "5003d67e0b1362696e616e63652f38303499008301359ed78efabe6d6d99fe4e5b8988d90b863c1a90ab7b8f5b6ebd24d31a7835014839aefa3d39897904000000000000000000a5a001e9170000000000",
        "034d0000",
        "4e03acb70b11627463636f6d383930e1023b025de50debfabe6d6d105d17bbc86f999c3b578d9696815d8bba132206863eceebaf339f9bbc01486b04000000000000000000a5fa4c00000000000000",
        // Tx in block 500531
        "1f0333a307122f3538636f696e2e636f6d2f16205a3cfb3dbe4f0000d84e0100",
        // Tx[0].inputs[0] in block 500000
        "4b0320a107046f0a385a632f4254432e434f4d2ffabe6d6dbdd0ee86f9a1badfd0aa1b3c9dac8d90840cf973f7b2590d6c9adde1a6e0974a010000000000000001283da9a172020000000000",
        // Tx[1].outputs[2] in block 265458
        "014c"

    };
    // Should be the scriptpubkey_asm or scriptsig_asm value from
    // blockstream.info
    char expect_asm[][buf_size] = {
        // OP_PUSHDATA at the end, with no bytes representing the length of operand.
        "OP_RETURN_252 OP_2OVER OP_PUSHBYTES_3 5c7a81 OP_RETURN_188 OP_3DUP OP_RETURN_204 OP_PUSHBYTES_54 947f7c1b2d63620560bec2aa336a676213bab74c9f03d46788100dca84c0f19a0f1c14ef0d67f3fc63c011ba4787510d55fde9554e55<unexpected end>",
        // OP_PUSHDATA at the end
        "OP_RETURN OP_PUSHDATA1 52534b424c4f434b3a3f6536dbb51cbe76519e0cd70480cf7f07da4ae2334cac402ef18329004681c4",
        // OP_PUSHDATA that pushes more bytes than available
        "OP_PUSHBYTES_3 d67e0b OP_PUSHBYTES_19 62696e616e63652f38303499008301359ed78e OP_RETURN_250 OP_RETURN_190 OP_2DROP OP_2DROP OP_RSHIFT OP_RETURN_254 OP_PUSHDATA4 <push past end>",
        // valid OP_PUSHDATA only
        "OP_PUSHDATA2",
        // OP_PUSHDATA that pushes nothing
        "OP_PUSHBYTES_3 acb70b OP_PUSHBYTES_17 627463636f6d383930e1023b025de50deb OP_RETURN_250 OP_RETURN_190 OP_2DROP OP_2DROP OP_PUSHBYTES_16 5d17bbc86f999c3b578d9696815d8bba OP_PUSHBYTES_19 2206863eceebaf339f9bbc01486b0400000000 OP_0 OP_0 OP_0 OP_0 OP_0 OP_WITHIN OP_RETURN_250 OP_PUSHDATA1 OP_0 OP_0 OP_0 OP_0 OP_0 OP_0",
        // Combination of <unexpected end> and less bytes for OP_PUSHDATA than expected
        "OP_PUSHBYTES_3 33a307 OP_PUSHBYTES_18 2f3538636f696e2e636f6d2f16205a3cfb3d OP_RETURN_190 OP_PUSHNUM_NEG1 OP_0 OP_0 OP_RETURN_216<unexpected end>",
        // <push past end> without OP_PUSHDATA
        "OP_PUSHBYTES_3 20a107 OP_PUSHBYTES_4 6f0a385a OP_IF OP_PUSHBYTES_47 4254432e434f4d2ffabe6d6dbdd0ee86f9a1badfd0aa1b3c9dac8d90840cf973f7b2590d6c9adde1a6e0974a010000 OP_0 OP_0 OP_0 OP_0 OP_0 OP_PUSHBYTES_1 28 OP_PUSHBYTES_61 <push past end>",
        // OP_PUSHDATA followed by nothing
        "<unexpected end>"
        
    };
    const size_t expected_cmds_size[] = {9, 3, 10, 2, 23, 9, 11, 2};
    // Should exclude the length of operand (e.g., OP_PUSHBYTES_3) but
    // include the last operand even if its length is not expected.
    char expected_str_outs[][32][2048] = {
        {"fc", "70", "5c7a81", "bc", "6f", "cc", "947f7c1b2d63620560bec2aa336a676213bab74c9f03d46788100dca84c0f19a0f1c14ef0d67f3fc63c011ba4787510d55fde9554e55", "4e", ""},
        {"6a", "4c", "52534b424c4f434b3a3f6536dbb51cbe76519e0cd70480cf7f07da4ae2334cac402ef18329004681c4"},
        {"d67e0b", "62696e616e63652f38303499008301359ed78e", "fa", "be", "6d", "6d", "99", "fe", "4e", "0b863c1a90ab7b8f5b6ebd24d31a7835014839aefa3d39897904000000000000000000a5a001e9170000000000"},
        {"4d", ""},
        {"acb70b", "627463636f6d383930e1023b025de50deb", "fa", "be", "6d", "6d", "5d17bbc86f999c3b578d9696815d8bba", "2206863eceebaf339f9bbc01486b0400000000", "00", "00", "00", "00", "00", "a5", "fa", "4c", "", "00", "00", "00", "00", "00", "00"},
        {"33a307", "2f3538636f696e2e636f6d2f16205a3cfb3d", "be", "4f", "00", "00", "d8", "4e", ""},
        {"20a107", "6f0a385a", "63", "4254432e434f4d2ffabe6d6dbdd0ee86f9a1badfd0aa1b3c9dac8d90840cf973f7b2590d6c9adde1a6e0974a010000", "00", "00", "00", "00", "00", "28", "a9a172020000000000"},
        {"4c", ""}
    };
    for (size_t i = 0; i < sizeof(hex_str_in)/sizeof(hex_str_in[0]); ++i) {
        int retval = test_parsing_and_serialization_and_get_asm(hex_str_in[i],
            expected_cmds_size[i], expected_str_outs[i], expect_asm[i]);
        if (retval != 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    int retval = 0;

    struct Test_Suite {
        char test_name[128];
        int (*test_func)(void);
    };
    
    struct Test_Suite test_suites[] = {
        {"test_op_dup()", &test_op_dup},
        {"test_op_hash256()", &test_op_hash256},
        {"test_op_hash160()", &test_op_hash160},
        {"test_script_parsing_and_serialization1()", &test_script_parsing_and_serialization1},
        {"test_script_parsing_and_serialization2()", &test_script_parsing_and_serialization2},
        {"test_script_parsing_and_serialization3_OP_PUSHDATA1()", &test_script_parsing_and_serialization3_OP_PUSHDATA1},
        {"test_script_parsing_and_serialization4_OP_PUSHDATA2()", &test_script_parsing_and_serialization4_OP_PUSHDATA2},
        {"test_script_parsing_and_serialization5_OP_PUSH()", &test_script_parsing_and_serialization5_OP_PUSH},
        {"test_script_parsing_and_serialization6_special_cases()", &test_script_parsing_and_serialization6_special_cases}
    };

    for (uint32_t i = 0; i < sizeof(test_suites)/sizeof(test_suites[0]); ++i) {
        printf("testing %s...\n", test_suites[i].test_name);
        if (test_suites[i].test_func() != 0) {
            ++retval;
            fprintf(stderr, "FAILED!!!\n");
        }
    }

    if (retval != 0) {
        fprintf(stderr, "===== %d TEST(s) FAILED!!! =====\n", retval);
    } else {
        printf("All tests passed\n");
    }
    return retval;
}
