#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "../tx.h"
#include "../utils.h"
#include "../cryptographic-algorithms/src/misc.h"

using namespace std;


int main() {
  size_t input_len;
  char* hex_input = (char*)hex_string_to_bytes(
    "010000000299562c9e493cdc95891e07ae4258607f6430ca4156f671ce439cdbb16d4de92c010000006a473044022042c4c922893d2ab0dffe32b05a95b231eae0d990d61833027cb33ed7c21292d702207aa3badea0abf374bdd8dfa70e9ce4e74fcddb4863b8510f500c49f7ea9f246d012103dc42b9159e5b5de99947906b91c89b7d81d1105350154a820d8ca832d777bb3dffffffff37d2209442bf821a42843c15ca34ffa5729d09a12efebb478edd54a0a0969d8f000000006b48304502210092c08ea55afedb5c4d681a34d1301ada6b6d4217298c8f182948fe8ed20772b4022064a6499ebf0bc8754306c2779b8e7e8f3eab929ad8c2705a7f5f8ad1d71d428a012102c907792e2d6fa4177fd649b79ac3cef2350d4514452a86a21b0fe45732000927ffffffff02b72da206000000001976a914e09aea4f80c154d31d53baa89ca9fd8cd868f70388ac32bff102000000001976a91420bc690956523bba61f1a895d82910f3e7523f1f88ac00000000",
    &input_len
  );
  stringstream ss;
  ss.write(hex_input, input_len);  
  free(hex_input);
  Tx my_tx = Tx();
  my_tx.parse(&ss);
  printf("version(): %u\n", my_tx.get_version());
  printf("get_tx_in_count(): %u\n", my_tx.get_tx_in_count());  
  printf("get_tx_out_count(): %u\n", my_tx.get_tx_out_count());
  return 0;
}