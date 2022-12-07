#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "tx.h"
#include "utils.h"
#include "./cryptographic-algorithms/src/misc.h"

using namespace std;

Test(ch05_test_suite, test_varint_encoding_decoding1) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);

  var_int = encode_variable_int(0, &int_len);
  cr_assert(int_len == 1);
  cr_assert(eq(u8, var_int[0], 0));  
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 0));
}

Test(ch05_test_suite, test_varint_encoding_decoding2) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);

  var_int = encode_variable_int(1234, &int_len);
  cr_assert(eq(u32, int_len, 3));
  uint8_t expected[] = {0xfd,0xd2,0x04};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 1234));
}

Test(ch05_test_suite, test_varint_encoding_decoding4) { 
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);

  var_int = encode_variable_int(1048575, &int_len);
  cr_assert(eq(u32, int_len, 5));
  uint8_t expected[] = {0xfe,0xff,0xff,0x0f,0x00};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 1048575));
}

Test(ch05_test_suite, test_varint_encoding_decoding5) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);
 
  var_int = encode_variable_int(31415926, &int_len);
  cr_assert(eq(u32, int_len, 5));
  uint8_t expected[] = {0xfe,0x76,0x5e,0xdf,0x01};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 31415926));
}

Test(ch05_test_suite, test_varint_encoding_decoding6) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(8);
  
  var_int = encode_variable_int(2147483648, &int_len);
  cr_assert(eq(u32, int_len, 5));
  uint8_t expected[] = {0xfe,0x00,0x00,0x00,0x80};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 2147483648));
}

Test(ch05_test_suite, test_varint_encoding_decoding7) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(9);

  var_int = encode_variable_int(4294967296, &int_len);
  cr_assert(eq(u32, int_len, 9));
  uint8_t expected[] = {0xff,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 4294967296));
}

Test(ch05_test_suite, test_varint_encoding_decoding8) {
  uint8_t* var_int;
  size_t int_len = -1;
  uint64_t result = 0;
  vector<uint8_t> bytes(9);

  var_int = encode_variable_int(18446744073709551615u, &int_len);; // 0xffffffffffffffffff, max value of uint64_t
  cr_assert(eq(u32, int_len, 9));
  uint8_t expected[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  for (size_t i = 0; i < int_len; ++i) {
    cr_assert(eq(u8, var_int[i], expected[i]));  
  }
  memcpy(bytes.data(), var_int, int_len);
  result = read_variable_int(bytes);
  free(var_int);
  cr_assert(eq(u64, result, 18446744073709551615u));
}

Test(ch05_test_suite, test_parse1) {
  size_t input_len;
  // The input hex string is from the book
  uint8_t* hex_input = hex_string_to_bytes(
    "0100000001813f79011acb80925dfe69b3def355fe914bd1d96a3f5f71bf8303c6a989c7d1000000006b483045022100ed81ff192e75a3fd2"
    "304004dcadb746fa5e24c5031ccfcf21320b0277457c98f02207a986d955c6e0cb35d446a89d3f56100f4d7f67801c31967743a9c8e10615b"
    "ed01210349fc4e631e3624a545de3f89f5d8684c7b8138bd94bdd531d2e213bf016b278afeffffff02a135ef01000000001976a914bc3b654"
    "dca7e56b04dca18f2566cdaf02e8d9ada88ac99c39800000000001976a9141c4bc762dd5423e332166702cb75f40df79fea1288ac19430600",
    &input_len
  );
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_input, input_len);
  free(hex_input);  
  Tx my_tx = Tx();
  my_tx.parse(d);
  cr_assert(eq(u32, my_tx.get_version(), 1u));
  cr_assert(eq(u32, my_tx.get_tx_in_count(), 1u));
  vector<TxIn> tx_ins = my_tx.get_tx_ins();
  uint8_t* prev_tx_id = tx_ins[0].get_prev_tx_id();
  uint8_t expected[] = {0xd1,0xc7,0x89,0xa9,0xc6,0x03,0x83,0xbf,0x71,0x5f,0x3f,0x6a,0xd9,0xd1,0x4b,0x91,0xfe,0x55,0xf3,0xde,0xb3,0x69,0xfe,0x5d,0x92,0x80,0xcb,0x1a,0x01,0x79,0x3f,0x81};
  cr_assert(sizeof(expected)/sizeof(uint8_t) == SHA256_HASH_SIZE);
  for (int i = 0; i < SHA256_HASH_SIZE; ++i) {
    cr_assert(eq(u8, prev_tx_id[i], expected[i]));
  }
  cr_assert(eq(u32, tx_ins[0].get_prev_tx_idx(), 0u));
  cr_assert(eq(u32, tx_ins[0].get_sequence(), 0xfffffffeu));

  cr_assert(eq(u32, my_tx.get_tx_out_count(), 2u));
  
  vector<TxOut> tx_outs = my_tx.get_tx_outs();
  cr_assert(eq(u64, tx_outs[0].get_amount(), 32454049u));
  cr_assert(eq(u64, tx_outs[1].get_amount(), 10011545));
  cr_assert(eq(u32, my_tx.get_locktime(), 410393u));
}

Test(ch05_test_suite, test_parse2) {
  size_t input_len;
  uint8_t* hex_input = hex_string_to_bytes(
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
  );
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_input, input_len);
  free(hex_input);

  Tx my_tx = Tx();
  my_tx.parse(d);

  vector<TxOut> tx_outs = my_tx.get_tx_outs();
  cr_assert(eq(u32, tx_outs[1].get_amount(), 40000000u));
}

Test(ch05_test_suite, test_curl_fetch_mainnet) {
  Tx my_tx = Tx();
  uint8_t tx_id[] = {0xb2,0x28,0x1f,0x1f,0x0e,0x0d,0x19,0x04,0x72,0xbd,0x02,0x51,0xef,0x6c,0x29,0x74,0xc9,0xad,0xeb,0x9e,0x5c,0x0c,0x3b,0xee,0x80,0x33,0xe9,0xb5,0x91,0x7d,0xe3,0xd0};
  // https://blockstream.info/tx/b2281f1f0e0d190472bd0251ef6c2974c9adeb9e5c0c3bee8033e9b5917de3d0
  // https://blockstream.info/api/tx/b2281f1f0e0d190472bd0251ef6c2974c9adeb9e5c0c3bee8033e9b5917de3d0/hex
  bool retval = my_tx.fetch(tx_id, false);
  char expected[] = "010000000e06e3df9440f53211a7110b91f99e844c6200dc0265789b7704b8787dd57fd28f010000008b4830450220080642b1df56ace40bbafa25e5752fde2ef0c16e5163e2927ae8c2021ca99f02022100aa5092591143f8db1d4b400220f4d207072cb1fdc9b5c61162e0d730541cb62501410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffff21269150db2a13d5c9ac4fd30d1beb578adc108997c28ab8016a75404e006970010000008c4930460221008facd8465d6df0b3eab092039673846d404161b41b2fdd4bdae1e487352eca7a02210098942f5782741714387d2d75b0ff0d8fdfdfe21e2653980997cd87d864a8d62101410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffff256d8553239c26036c4f2f4945681c5b44d7a4a1474d523616b3a6e743faa025010000008b48304502210085ab99d83fc3ce3dd1fda9b1d240ecfcf401055a66ddcefe821213d740b4a01e0220736e1aedf06e5c5105a9284fd1d364dd9e607f67f50925ba84bf71153280a63001410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffff39089d603546b25d267ac259d15f577c1f26b348c0704213bedfe437aaa77dbc010000008b483045022100957cf87ce67e731a831ac92090ec9a00b8125de0f5ce5ae473123a37ebf405820220326db480040e6be2fed3a6927f9f4e8bed95a2a4dfdc0b594a8c74b12e533aea01410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffff43da501170f55820d45fe64146ccfdb875e1ff2293e0dd77e6eab4b0c19e7a4c010000008b483045022100d0efa36cf88860a2fbb348d80714fc900e719cf459310d4a3139cfe75421a830022015f71d8f26f1422fa3bc63ee9c6a282fc5792f2298caf2bda2cf3177b6fbcb3201410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffff82fc3cc693749c7a9c35fa885e04d473c979dfcf84ec291b509fea57b70d55e8010000008b483045022100efa194c08025ba9d04fd7da6ceb63edebc45d559ddfc0edeb47208dd158950f2022030a7063b3f53342a5ee099b374763cb83c353ddffb214a9591b05b15bca5d43401410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffff8869f53f79c0f27b3cc9b1c0fed067a411a376951aab87893ac134189bf4dcb1010000008b483045022100ef848352d26950ebce11be68e5bf4ba8abaf55d2f93dff744c0ea209326a1af8022006859ba26aa46d6ef833f2317ea7db8c0d4b938681ec39fc17aa6310e7b0131701410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffffaf91fcd2c0f23eed9e03ff1c12c59099be0619c17bde7e1cb91e72955d8d0054010000008a4730440220461088a2cccfffd0d5b1571e15c17f20124fce6e9bfc984af66d4da64a9045ba022054910c8ce8fbd309ac71fae66d94cb99a0b63ae6f6b88846f41f463930f2e0a601410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffffe17bda370245afeea57e2768f03b0a53a1286ce56356a65792dcfefd4f4e0878010000008c493046022100b8d16cd485ad47a49663bfa6def0c1dc1bc6e8932ad54150d8e0061a6c147157022100cb3c1b57b3af471774cf782ff728c4b347a7b1c4af74b73796f01fdab7cf838601410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffffe1a4cdab23a73ae8f0d01e7d9869965fed600e28b6e5a0d87916ae9a1d1fef84010000008a47304402200bc9e0d336ee6dc5f6882c405b5b7782502d2866c6d06ec0a7a68131dd2202cb022054c69fde9ba3f1e2c3e03e413f26fa1eeb87b991047f31211997bc5425e5594701410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffffe79e2d37b403be66705a21b2e215c394e423510a12529011b85b24889369190a010000008c493046022100d5b1263af96cff8f1d8b689183301d785cfab29bc5b98a15d6fc71025aa8e714022100d7499e22310aad1129ab2383f6549c7e403137a83bdfc28789f1e4b068fa6fc901410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffffefee1c7d0c46c1f4559d76050a13c226503798414c9d53eaf47d2542d1d66caf010000008a47304402201efbb446cc6e78fb2470e12ada2f7ecb8f4237e34cf550dea7f4d4aaa8ba0d410220426d7e380fa0517cf5f52fa83b082a1b067a9b4c4ed3da88329f190cfee3816c01410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2fffffffffc1c8e67506a75e9555b44782da3920b510ed1ca40a61f3d6cb8e71873272231010000008b483045022100ccb6b7d8b882697087cd0f4038563381a50c329589803fdbfdcb3af3da631b8c02202806f0da3683576e0106385486e407e004135c45dc2f82633bcc4af17a9ea5d501410436a74a2bbb3b15dc98e42fe84613bad7e530a27fd6c243692f2e1a2616cfc042f4e0e9faa9986e3ea83b119ccffa527267b734501ab9cc7b0eae75195184b4a2ffffffffbd8d0ffd9948a03d690ba3c5623e96ea934e4f757e928702215bf6ed0de55ecf010000008c493046022100ade7d2e6403521c99284396929b62003a8c0d67e16496acbce8ecefa7957ba91022100c520400d6e1dd8081dd142e59811d1d9720243590302a4337c60840c496f9ee00141049c9727f45ca04ffd2808ee76eb9381f7260b66a8d0c78df3b9453f7d0be2418b7ab300b5c5e4f6771d8a557a8b949b245e46b01df7caba8574f299c805d56925ffffffff02d0f0e201000000001976a9143fc92964be4848caf2ea4c7735456efa4cc2a19388ac006d7c4d000000001976a914e9aa9336369630b5381422bda93771db567dc00f88ac00000000";
  cr_assert(eq(u32, sizeof(expected)/sizeof(expected[0]), my_tx.get_curl_buffer_end()));
  cr_assert(eq(i32, retval, 1));
  for (size_t i = 0; i < sizeof(expected)/sizeof(expected[0]); ++i) {
    cr_assert(eq(u8, expected[i], my_tx.get_curl_buffer()[i]));
  }
}

Test(ch05_test_suite, test_curl_fetch_testnet) {
  Tx my_tx = Tx();
  uint8_t tx_id[] = {0xf0,0x63,0x5c,0xe1,0x5b,0x7c,0x00,0xd4,0x6c,0x61,0x53,0xe2,0x2b,0x20,0x6b,0xda,0x6b,0x64,0x72,0x7d,0x09,0xb8,0x10,0xbe,0xa4,0xc3,0xcd,0xe5,0x84,0xad,0x03,0x8d};
  // https://blockstream.info/tx/f0635ce15b7c00d46c6153e22b206bda6b64727d09b810bea4c3cde584ad038d
  // https://blockstream.info/testnet/api/tx/f0635ce15b7c00d46c6153e22b206bda6b64727d09b810bea4c3cde584ad038d/hex
  bool retval = my_tx.fetch(tx_id, true);
  char expected[] = "010000000001015058e90937c45dc172f88a31e491e916576a63d567ab6adb51f4e3d7cf1f607e0100000000ffffffff02c9730000000000001976a91466177bdad6c673450bb33de91a698b7648e682c188ac96000a00000000001600146ab1439c6c433546f1655b60a639e3b5e8a6c45d024730440220734437ece723e576a026e7e126412421036d73a567a38af71c4f11bbed555ec902201020f2ea0cf881c22c160d3ab8235af2c2d3b389e24679186cd51072d9d98dc6012103846f7366e4f7818ec17e6d13b1e947fe102ae7a6061a3be50e7388b335b1c78800000000";
  cr_assert(eq(u32, sizeof(expected)/sizeof(expected[0]), my_tx.get_curl_buffer_end()));
  cr_assert(eq(i32, retval, 1));
  for (size_t i = 0; i < sizeof(expected)/sizeof(expected[0]); ++i) {
    cr_assert(eq(u8, expected[i], my_tx.get_curl_buffer()[i]));
  }
}


Test(ch05_test_suite, test_parse_fee1) {
  size_t input_len;
  uint8_t* hex_input = hex_string_to_bytes(
    "0100000001813f79011acb80925dfe69b3def355fe914bd1d96a3f5f71bf8303c6a989c7d1000000006b483045022100ed81ff192e75a3fd2"
    "304004dcadb746fa5e24c5031ccfcf21320b0277457c98f02207a986d955c6e0cb35d446a89d3f56100f4d7f67801c31967743a9c8e10615b"
    "ed01210349fc4e631e3624a545de3f89f5d8684c7b8138bd94bdd531d2e213bf016b278afeffffff02a135ef01000000001976a914bc3b654"
    "dca7e56b04dca18f2566cdaf02e8d9ada88ac99c39800000000001976a9141c4bc762dd5423e332166702cb75f40df79fea1288ac19430600",
    &input_len
  );
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_input, input_len);
  free(hex_input);
  Tx my_tx = Tx();
  my_tx.parse(d);
  cr_assert(eq(u32, my_tx.get_fee(), 40000u));
}

Test(ch05_test_suite, test_parse_fee2) {
  
  size_t input_len;
  uint8_t* hex_input = hex_string_to_bytes(
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
  );
  vector<uint8_t> d(input_len);
  memcpy(d.data(), hex_input, input_len);
  free(hex_input);
  Tx my_tx = Tx();
  my_tx.parse(d);  
  cr_assert(eq(u32, my_tx.get_fee(), 140500u));
}