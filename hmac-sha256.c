/*
   hmac_sha256.c
   Originally written by https://github.com/h5p9sl
 */

#include "hmac-sha256.h"
#include "sha256.h"


#include <stdlib.h>
#include <string.h>

// Concatenate X & Y, return hash.
static void* H(const void* x, const size_t xlen, const void* y, const size_t ylen, void* out);


void hmac_sha256(const void* key, const size_t key_len, const void* msg, const size_t msg_len, void* out) {
  uint8_t k_prime[SHA256_BLOCK_SIZE];
  uint8_t ipad[SHA256_BLOCK_SIZE]; // ipad is the block-sized inner padding, consisting of repeated bytes valued 0x36
  uint8_t opad[SHA256_BLOCK_SIZE]; // opad is the block-sized outer padding, consisting of repeated bytes valued 0x5c
  uint8_t ihash[SHA256_HASH_SIZE];
  uint8_t ohash[SHA256_HASH_SIZE];
  int i;

  memset(k_prime, 0, sizeof(k_prime));
  memset(ipad, 0x36, SHA256_BLOCK_SIZE);
  memset(opad, 0x5c, SHA256_BLOCK_SIZE);

  if (key_len > SHA256_BLOCK_SIZE) { calc_sha_256(k_prime, key, key_len); }
  else { memcpy(k_prime, key, key_len); }

  for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
    ipad[i] ^= k_prime[i]; // ^: bitwise XOR
    opad[i] ^= k_prime[i];
  }

  // Perform HMAC algorithm: ( https://tools.ietf.org/html/rfc2104 )
  //      `H(K XOR opad, H(K XOR ipad, data))`
  H(ipad, SHA256_BLOCK_SIZE, msg, msg_len, ihash);
  H(opad, SHA256_BLOCK_SIZE, ihash, SHA256_HASH_SIZE, ohash);

  memcpy(out, ohash, SHA256_HASH_SIZE);
}

static void* H(const void* x, const size_t xlen, const void* y, const size_t ylen, void* out) {
  size_t buflen = (xlen + ylen);
  uint8_t* buf = (uint8_t*)malloc(buflen);

  memcpy(buf, x, xlen);
  memcpy(buf + xlen, y, ylen);
  calc_sha_256(out, buf, buflen);

  free(buf);
}