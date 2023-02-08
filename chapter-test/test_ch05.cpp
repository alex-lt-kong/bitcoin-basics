#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <mycrypto/misc.hpp>

#include "mybitcoin/tx.h"
#include "mybitcoin/utils.h"


using namespace std;

int test_varint_encoding_decoding1() {
    uint8_t* var_int;
    size_t int_len = -1;
    uint64_t result = 0;
    vector<uint8_t> bytes(8);

    var_int = encode_variable_int(0, &int_len);
    if (int_len != 1) {
        return 1;
    }
    if (var_int[0] != 0) {
        return 1;
    }
    memcpy(bytes.data(), var_int, int_len);
    result = read_variable_int(bytes);
    free(var_int);
    if (result != 0) {
        return 1;
    }
    return 0;
}

int test_varint_encoding_decoding2() {
    uint8_t* var_int;
    size_t int_len = -1;
    uint64_t result = 0;
    vector<uint8_t> bytes(8);

    var_int = encode_variable_int(1234, &int_len);
    if (int_len != 3) {
      return 1;
    }
    uint8_t expected[] = {0xfd,0xd2,0x04};
    for (size_t i = 0; i < int_len; ++i) {
        if (var_int[i] != expected[i]) {
          return 1;
        }
    }
    memcpy(bytes.data(), var_int, int_len);
    result = read_variable_int(bytes);
    free(var_int);
    if (result != 1234) {
      return 1;
    }
    return 0;
}

int test_varint_encoding_decoding4() { 
    uint8_t* var_int;
    size_t int_len = -1;
    uint64_t result = 0;
    vector<uint8_t> bytes(8);

    var_int = encode_variable_int(1048575, &int_len);
    if (int_len != 5) {
        return 1;
    }
    uint8_t expected[] = {0xfe,0xff,0xff,0x0f,0x00};
    for (size_t i = 0; i < int_len; ++i) {
        if (var_int[i] != expected[i]) {
            return 1;
        }
    }
    memcpy(bytes.data(), var_int, int_len);
    result = read_variable_int(bytes);
    free(var_int);
    if (result != 1048575) {
        return 1;
    }
    return 0;
}

int test_varint_encoding_decoding5() {
    uint8_t* var_int;
    size_t int_len = -1;
    uint64_t result = 0;
    vector<uint8_t> bytes(8);
 
    var_int = encode_variable_int(31415926, &int_len);
    if (int_len != 5) {
        return 1;
    }
    uint8_t expected[] = {0xfe,0x76,0x5e,0xdf,0x01};
    for (size_t i = 0; i < int_len; ++i) {
        if (var_int[i] != expected[i]) {
            return 1;
        }
    }
    memcpy(bytes.data(), var_int, int_len);
    result = read_variable_int(bytes);
    free(var_int);
    if (result != 31415926) {
        return 1;
    }
    return 0;
}

int test_varint_encoding_decoding6() {
    uint8_t* var_int;
    size_t int_len = -1;
    uint64_t result = 0;
    vector<uint8_t> bytes(8);
    
    var_int = encode_variable_int(2147483648, &int_len);
    if (int_len != 5) {
        return 1;
    }
    uint8_t expected[] = {0xfe,0x00,0x00,0x00,0x80};
    for (size_t i = 0; i < int_len; ++i) {
        if (var_int[i] != expected[i]) {
            return 1;
        }
    }
    memcpy(bytes.data(), var_int, int_len);
    result = read_variable_int(bytes);
    free(var_int);
    if (result != 2147483648) {
        return 1;
    }
    return 0;
}

int test_varint_encoding_decoding7() {
    uint8_t* var_int;
    size_t int_len = -1;
    uint64_t result = 0;
    vector<uint8_t> bytes(9);

    var_int = encode_variable_int(4294967296, &int_len);
    if (int_len != 9) {
        return 1;
    }
    uint8_t expected[] = {0xff,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
    for (size_t i = 0; i < int_len; ++i) {
        if (var_int[i] != expected[i]) {
            return 1;
        }
    }
    memcpy(bytes.data(), var_int, int_len);
    result = read_variable_int(bytes);
    free(var_int);
    if (result != 4294967296) {
        return 1;
    }
    return 0;
}

int test_varint_encoding_decoding8() {
    uint8_t* var_int;
    size_t int_len = -1;
    uint64_t result = 0;
    vector<uint8_t> bytes(9);

    var_int = encode_variable_int(18446744073709551615u, &int_len);; // 0xffffffffffffffffff, max value of uint64_t
    if (int_len != 9) {
        return 1;
    }
    uint8_t expected[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    for (size_t i = 0; i < int_len; ++i) {
        if (var_int[i] != expected[i]) {
            return 1;
        }
    }
    memcpy(bytes.data(), var_int, int_len);
    result = read_variable_int(bytes);
    free(var_int);
    if (result != 18446744073709551615u) {
        return 1;
    }
    return 0;
}

int test_parse1() {
    int64_t input_len;
    // The input hex string is from the book
    unique_byte_ptr hex_input(hex_string_to_bytes(
        "0100000001813f79011acb80925dfe69b3def355fe914bd1d96a3f5f71bf8303c6a989c7d1000000006b483045022100ed81ff192e75a3fd2"
        "304004dcadb746fa5e24c5031ccfcf21320b0277457c98f02207a986d955c6e0cb35d446a89d3f56100f4d7f67801c31967743a9c8e10615b"
        "ed01210349fc4e631e3624a545de3f89f5d8684c7b8138bd94bdd531d2e213bf016b278afeffffff02a135ef01000000001976a914bc3b654"
        "dca7e56b04dca18f2566cdaf02e8d9ada88ac99c39800000000001976a9141c4bc762dd5423e332166702cb75f40df79fea1288ac19430600",
        &input_len
    ));
    if (hex_input == NULL) {     
        return 1;
    }
    
    vector<uint8_t> d(input_len);
    memcpy(d.data(), hex_input.get(), input_len);
    Tx my_tx = Tx(d);
    if (my_tx.get_version() != 1u) {
        return 1;
    }
    if (my_tx.get_tx_in_count() != 1u) {
        return 1;
    }
    vector<TxIn> tx_ins = my_tx.get_tx_ins();
    uint8_t* prev_tx_id = tx_ins[0].get_prev_tx_id();
    uint8_t expected[] = {0xd1,0xc7,0x89,0xa9,0xc6,0x03,0x83,0xbf,0x71,0x5f,0x3f,0x6a,0xd9,0xd1,0x4b,0x91,0xfe,0x55,0xf3,0xde,0xb3,0x69,0xfe,0x5d,0x92,0x80,0xcb,0x1a,0x01,0x79,0x3f,0x81};
    if (sizeof(expected)/sizeof(uint8_t) != SHA256_HASH_SIZE) {
        return 1;
    }
    for (int i = 0; i < SHA256_HASH_SIZE; ++i) {
        if (prev_tx_id[i] != expected[i]) {
            return 1;
        }
    }
    if (tx_ins[0].get_prev_tx_idx() != 0u) {
        return 1;
    }
    if (tx_ins[0].get_sequence() != 0xfffffffeu) {
        return 1;
    }

    if (my_tx.get_tx_out_count() != 2u) {
        return 1;
    }
    
    vector<TxOut> tx_outs = my_tx.get_tx_outs();
    if (tx_outs[0].get_amount() != 32454049u) {
        return 1;
    }
    if (tx_outs[1].get_amount() != 10011545) {
        return 1;
    }
    if (my_tx.get_locktime() != 410393u) {
        return 1;
    }
    return 0;
}

int test_parse2() {
    int64_t input_len;
    unique_byte_ptr hex_input(hex_string_to_bytes(
        "010000000456919960ac691763688d3d3bcea9ad6ecaf875df5339e148a1fc61c6ed7a069e010000006a47304402204585bcdef85e6b1c6af"
        "5c2669d4830ff86e42dd205c0e089bc2a821657e951c002201024a10366077f87d6bce1f7100ad8cfa8a064b39d4e8fe4ea13a7b71aa8180f"
        "012102f0da57e85eec2934a82a585ea337ce2f4998b50ae699dd79f5880e253dafafb7feffffffeb8f51f4038dc17e6313cf831d4f02281c2"
        "a468bde0fafd37f1bf882729e7fd3000000006a47304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc26125"
        "022008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a7160121035d5c93d9ac96881f19ba1f686f15f009ded7c62"
        "efe85a872e6a19b43c15a2937feffffff567bf40595119d1bb8a3037c356efd56170b64cbcc160fb028fa10704b45d775000000006a473044"
        "02204c7c7818424c7f7911da6cddc59655a70af1cb5eaf17c69dadbfc74ffa0b662f02207599e08bc8023693ad4e9527dc42c34210f7a7d1d"
        "1ddfc8492b654a11e7620a0012102158b46fbdff65d0172b7989aec8850aa0dae49abfb84c81ae6e5b251a58ace5cfeffffffd63a5e6c16e6"
        "20f86f375925b21cabaf736c779f88fd04dcad51d26690f7f345010000006a47304402200633ea0d3314bea0d95b3cd8dadb2ef79ea8331ff"
        "e1e61f762c0f6daea0fabde022029f23b3e9c30f080446150b23852028751635dcee2be669c2a1686a4b5edf304012103ffd6f4a67e94aba3"
        "53a00882e563ff2722eb4cff0ad6006e86ee20dfe7520d55feffffff0251430f00000000001976a914ab0c0b2e98b1ab6dbf67d4750b0a562"
        "44948a87988ac005a6202000000001976a9143c82d7df364eb6c75be8c80df2b3eda8db57397088ac46430600",
        &input_len
    ));
    vector<uint8_t> d(input_len);
    memcpy(d.data(), hex_input.get(), input_len);

    Tx my_tx = Tx(d);

    vector<TxOut> tx_outs = my_tx.get_tx_outs();
    if (tx_outs[1].get_amount() != 40000000u) {
        return 1;
    }
    return 0;
}

int test_parse3() {
    // txid: 50ef8c53f217198bbd50e77a7a080d3ccb46d08780cd8c2e270068026a60840b
    int64_t input_len;
    unique_byte_ptr hex_input(hex_string_to_bytes(
        "0100000004ed9bb5c1db8934939485b58f88f71b4977d58a9ef280dd45e29f8c177d080c2d010000006a473044022003cb49c9efd0f502e6ec41716b597ef6e6b007b6a9b7ebceb6c4419f3d98402d0220682899d703bd87a98b0b44ca617d1e9f68f3db13846b200fd36200157425ae5c012103481e3e7638e2c72f38a2cb21e81ac8206d9f2139c8376fca0a39c589ba0ae921ffffffffe58f1bee7f37ddf6ae23edfea86ab1e8f27331571dea7accc4258c26ed894d7d000000006a47304402206f0c159409be058069abb23fb8fedc165378ec50816fabbaa2b78519b0ba2e10022073e12cee385e6dda6b8b000f26f31f82c9ddd5488066a464b16f8c0642836fa50121031dfdd2c5618576996447ce0edbc3233ee7c278397dc586f9b33b72e87993cbf0ffffffffc5f0521022ac0d34d1002c64ebb42d425ae7bfc0d0a6b4fbed8ded7c7059d7b5010000006b483045022100ffba5b12f23c68ce456d128aa4117614b7a6441730eed2e6c60de5909f267046022024cd8fcfd12b14865bb781c3331bc20028f8aeeec5d89a7f7d32d68aaec5dbb3012103cd16e93c90bc8df69f714ad9e06cbb895062ac3546562a442d93fc757c156abfffffffff9ddc36c0b70c6ddf502aa18c8d622155ca93bd685deabe1b5b4cf82abf580950010000006a47304402202749576a899347ca6136e76f17390c91509393c23f412a731fc115fd8a77cd99022048536fcadc36d7ed67d2770ff602afd98d40225f5175640183e9d4e241c187b3012102df879c70e18abd9c40646b474d3c1520eb0fa7bc1e22d5f6e0ff02f34850771bffffffff02c4feef00000000001976a914129244290468fddfdf2f64abca98b7d687930baa88ac304cb7ee000000001976a914cabf367d97c39ffb8946273591bb40100c200a8d88ac00000000",
        &input_len
    ));
    vector<uint8_t> d(input_len);
    memcpy(d.data(), hex_input.get(), input_len);

    Tx my_tx = Tx(d);

    vector<TxOut> tx_outs = my_tx.get_tx_outs();
    if (tx_outs.size() != 2) {
        return 1;
    }
    if (tx_outs[0].get_amount() !=   15728324u) {
        return 1;
    }
     
    return 0;
}

int test_curl_fetch_mainnet() {
    Tx my_tx = Tx();
    char tx_id_hex[] = "b1d9ceea015b06c8753f48c0a04336719f00abbcecc5c1ed11a5c3005c587a0d";
    int64_t out_len;
    unique_byte_ptr tx_id(hex_string_to_bytes(tx_id_hex, &out_len));
    vector<uint8_t> d(64);
    bool retval = Tx::fetch_tx(tx_id.get(), d);
    char expected[] = "01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0804ffff001d028102ffffffff0100f2052a01000000434104a9f28305bd247981dd2a49abca2d8602110931298185128b43d7bcaea4863b93b8850cddbb7179c73d0cdccbc1623fae51f4b50b0a73d33f68f3cbe61658d96dac00000000";
    if (sizeof(expected)/sizeof(expected[0]) != d.size()) {
        printf("if (sizeof(expected)/sizeof(expected[0]) != d.size()) {\n\n");
        return 1;
    }
    if (retval != 0) {
        printf("retval != 0\n\n");
        return 1;
    }
    for (size_t i = 0; i < sizeof(expected)/sizeof(expected[0]); ++i) {
        if (expected[i] != d[i]) {
            printf("if (expected[i] != d[i]) {\n\n");
            return 1;
        }
    }
    return 0;
}

int test_parse_fee1() {
    int64_t input_len;
    // txid: 02d97b91fe9a25ae4975658e9229730491c34dae86ce87c28aaf927b32d8270c in block 205994
    unique_byte_ptr hex_input(hex_string_to_bytes(
        "01000000018568a1193f014dcfe68f3546391f9edd39f9001200c7e6d04a8d0484967b5faa000000008b483045022100ebfdadcc68e"
        "b2f6cfbecbe6cdd0b187d5e5c847b5edc1f4428a20c14449a869502207a7ead44e7b02ce3cc85fe8df11895b6d9eb0f19dd1a77a671"
        "0779552c2e350d014104144f87282d7d7007742a012ebcf5f665e050360accf9f3d5ac1a7c42ceb0c7cb4259800ed14ae742af6cbf6"
        "97af48f8e45474f9daa735adfee72b8ac090b6598ffffffff0240787d01000000001976a91406f1b6703d3f56427bfcfd372f952d50"
        "d04b64bd88ac688a6b01000000001976a91435235d510bd69d2981579f01da2ca669d9e26d5688ac00000000"
        ,
        &input_len
    ));
    vector<uint8_t> d(input_len);
    memcpy(d.data(), hex_input.get(), input_len);

    Tx my_tx = Tx(d);
    if (my_tx.get_fee() != 50000u) {
        return 1;
    }
    return 0;
}

int test_parse_fee2() {
    
    int64_t input_len;
    unique_byte_ptr hex_input(hex_string_to_bytes(
        "010000000456919960ac691763688d3d3bcea9ad6ecaf875df5339e148a1fc61c6ed7a069e010000006a47304402204585bcdef85e6b1c6a"
        "f5c2669d4830ff86e42dd205c0e089bc2a821657e951c002201024a10366077f87d6bce1f7100ad8cfa8a064b39d4e8fe4ea13a7b71aa818"
        "0f012102f0da57e85eec2934a82a585ea337ce2f4998b50ae699dd79f5880e253dafafb7feffffffeb8f51f4038dc17e6313cf831d4f0228"
        "1c2a468bde0fafd37f1bf882729e7fd3000000006a47304402207899531a52d59a6de200179928ca900254a36b8dff8bb75f5f5d71b1cdc2"
        "6125022008b422690b8461cb52c3cc30330b23d574351872b7c361e9aae3649071c1a7160121035d5c93d9ac96881f19ba1f686f15f009de"
        "d7c62efe85a872e6a19b43c15a2937feffffff567bf40595119d1bb8a3037c356efd56170b64cbcc160fb028fa10704b45d775000000006a"
        "47304402204c7c7818424c7f7911da6cddc59655a70af1cb5eaf17c69dadbfc74ffa0b662f02207599e08bc8023693ad4e9527dc42c34210"
        "f7a7d1d1ddfc8492b654a11e7620a0012102158b46fbdff65d0172b7989aec8850aa0dae49abfb84c81ae6e5b251a58ace5cfeffffffd63a"
        "5e6c16e620f86f375925b21cabaf736c779f88fd04dcad51d26690f7f345010000006a47304402200633ea0d3314bea0d95b3cd8dadb2ef7"
        "9ea8331ffe1e61f762c0f6daea0fabde022029f23b3e9c30f080446150b23852028751635dcee2be669c2a1686a4b5edf304012103ffd6f4"
        "a67e94aba353a00882e563ff2722eb4cff0ad6006e86ee20dfe7520d55feffffff0251430f00000000001976a914ab0c0b2e98b1ab6dbf67"
        "d4750b0a56244948a87988ac005a6202000000001976a9143c82d7df364eb6c75be8c80df2b3eda8db57397088ac46430600",
        &input_len
    ));
    vector<uint8_t> d(input_len);
    memcpy(d.data(), hex_input.get(), input_len);
    Tx my_tx = Tx(d);    
    if (my_tx.get_fee() != 140500u) {
        return 1;
    }
    return 0;
}

int test_parse_fee3() {
    
    int64_t input_len;
    unique_byte_ptr hex_input(hex_string_to_bytes(
        "010000000dbe5f67b4673f306e724cfa6d9af140aa1e28316696c5fe2150f916ff499a59ec800000008a47304402200219ad7dbed05d4f126c5817c6481a8a55c2584becb37d6eefd91ac5e57911350220324df1c14735cb75c2b7229a3a747f1992a9015539acc99b08ee3dbb01e2a0a3014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75fffffffff23e8cc59808923cf0213b6da6180dbf6736cf5647b701085eb91c56e8fc0c21790000008a47304402203244639f81d48ffff008332876e25418892dd71cfbf506a93b9175ecd94c161902204b4483ffdbdedb37c4e84299b37e7978f0aa770976eba5924e6c135e2f9e5a19014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff2382a7f1725bd4e6f7a3dabc1b83e26e325fd0012ff60e77264bbbc36d9da8d8780000008c4930460221009d1678b694c121eab575ba28e32dc8ce22e0f6663f662c720aabd168e85111e9022100c1651254c93a16d755a78be40b0f5df723ee2fa9bef2e43c29ff25682662c35e014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff749c85aaaff54f2482ff494e88a2f228b69667db676ab531ead19906b237e78e710000008c493046022100808de8d17245e16e087fe4fec2929e634a8045939ed70c990efc4e408448a6c502210087ec58b788c08ddda99e2a5003987a2743f7b02482d46a44fb3dac63541df0fa014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff02e30e92f11096c60b7e24752f692b69daf6c67722907e131897c467e9a8f33d5f0000008b48304502206670f328bffbbb5be3abf500b189e8e44e4500c2096365e1299d766cfda6058d0221009bcd7d3a44a4e0b71ea472b607f98261e6142dc3d56eb2aee184bfc3cc925906014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff36e497b7b51feb71df5043a096867d7b011ad03bf74c24f920763d2a965ff8ff6a0000008b48304502210083402b7f4f6092c0d2e693a77c438ef059a7a9fbd738e71040466146591b435302201eb2d497cad78ead02b3235d2280808d8a481a4b24762fe5a04ae44fa98204ca014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff000541e08b89dcd0f551cd964a89b14d58d5761601d3bf2a219677bfbb27418f000000008a47304402204a2b82b67cb671e1eab437a84aa66544c214ceb2c252bd04ca377d7f865ca8dd0220554cafa7c22c2666d5fd03e6c66bc7940dc1dcca2e1fccff54099ded63cf5ead014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75fffffffff7b8126418eb8a86c5a3a2ebdbd6a6ff41a7217841464394fd10f1d2ad65d7c96c0000008b483045022064ad60bffce2f746bea7a7cab3336c3244b1d63d625509fa95dc09d230559b51022100ef731c6f710590179d6bb905d91e5769b76b8c98fdb8395ada8d674c5db9cadc014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff45437b292187053ae16555e22680c83f173a78fe51cb3404004fb2ea6229da0c6d0000008c493046022100b521491a6228c2bbca6dc738f0270145d0b98718a97e1237b63c9b5ce1c332c3022100d8343a05861b24020b8a89e08ecacd3cedb1cab3237c3c7bab96e0cbec6b7866014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75fffffffff127a0b261ad461eac97ce6d478bca0a5565a491efa1042de22388bf1c69e0246d0000008c493046022100880a9f3ef8e004176d74db07e75f4e7210cef92e5c480836ac931b36f6df095b022100c69c059dbf2a56b4cd640113d919cbc3c891d1da0f1419a5c518c81228af9a39014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff760299786eedea7e77b8cb06e6d30cbbcaa773a99b2c29ed0f9b78f825dfdae3a90000008b48304502201bbb2d01eb30d9dabf93a620168c9bc742c55e10eb3be09e9469fa7133e7a332022100a9feabb6261073bc63c0849085e59966aac8dd96e5da32b5de4dc21aa351df06014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffffe10547885baf70249ed47668e258dfdeb37c01466fdafcc3387d31c52892fa75010000008b483045022100d7698344919ec7b91cb60f8cf0e4a2cf064140f1f8d21962c10f2467abdb6fee02200c4528caf3d7bb9517f8af6edca3ca0b61861ead73bc5df4b39d31f2935bf205014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff696d6ee3e4502dfe105541081f43b50de6e83461e593123e0c03fca319915bad010000008a4730440220155614eef455d51bf9557c4d9d5540de49b518be908afb158cbc03e6d8e96ad902201ae1ab0028a8c2f64dcb7efec804310fec630b067ad030128aec4678267a7163014104dad38844ebd5bb1a54b6285690139cb3cc5909414c84555594ba3b44bb675e3c9545f55ef8e4b367b5aa98b62fec82f3c8eab257631e9202f31785f29e35ac75ffffffff0154fcef02000000001976a914a8c398c47d0d42002fb665d73458a047220ef15388ac00000000",
        &input_len
    ));
    vector<uint8_t> d(input_len);
    memcpy(d.data(), hex_input.get(), input_len);

    Tx my_tx = Tx(d);    
    if (my_tx.get_fee() != 30000u) {
        return 1;
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
        {"test_varint_encoding_decoding1()", &test_varint_encoding_decoding1},
        {"test_varint_encoding_decoding2()", &test_varint_encoding_decoding2},
        {"test_varint_encoding_decoding4()", &test_varint_encoding_decoding4},
        {"test_varint_encoding_decoding5()", &test_varint_encoding_decoding5},
        {"test_varint_encoding_decoding6()", &test_varint_encoding_decoding6},
        {"test_varint_encoding_decoding7()", &test_varint_encoding_decoding7},
        {"test_varint_encoding_decoding8()", &test_varint_encoding_decoding8},
        {"test_parse1()", &test_parse1},
        {"test_parse2()", &test_parse2},
        {"test_parse3()", &test_parse3},
        {"test_curl_fetch_mainnet()", &test_curl_fetch_mainnet},
        {"test_parse_fee1()", &test_parse_fee1},
        {"test_parse_fee2()", &test_parse_fee2},
        {"test_parse_fee3()", &test_parse_fee3}
        
    };
    curl_global_init(CURL_GLOBAL_ALL);
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
    curl_global_cleanup();
    return retval;
}