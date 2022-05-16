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


void hmac_sha256(const void* key, const size_t keylen, const void* data, const size_t datalen, void* out) {
  uint8_t k[SHA256_BLOCK_SIZE];
  uint8_t k_ipad[SHA256_BLOCK_SIZE];
  uint8_t k_opad[SHA256_BLOCK_SIZE];
  uint8_t ihash[SHA256_HASH_SIZE];
  uint8_t ohash[SHA256_HASH_SIZE];
  int i;

  memset(k, 0, sizeof(k));
  memset(k_ipad, 0x36, SHA256_BLOCK_SIZE);
  memset(k_opad, 0x5c, SHA256_BLOCK_SIZE);

  if (keylen > SHA256_BLOCK_SIZE) {
    // If the key is larger than the hash algorithm's
    // block size, we must digest it first.
    calc_sha_256(k, key, keylen);
  } else {
    memcpy(k, key, keylen);
  }

  for (i = 0; i < SHA256_BLOCK_SIZE; i++) {
    k_ipad[i] ^= k[i];
    k_opad[i] ^= k[i];
  }

  // Perform HMAC algorithm: ( https://tools.ietf.org/html/rfc2104 )
  //      `H(K XOR opad, H(K XOR ipad, data))`
  H(k_ipad, sizeof(k_ipad), data, datalen, ihash);
  H(k_opad, sizeof(k_opad), ihash, sizeof(ihash), ohash);

  memcpy(out, ohash, SHA256_HASH_SIZE);
}

static void* H(const void* x,
               const size_t xlen,
               const void* y,
               const size_t ylen,
               void* out) {
  size_t buflen = (xlen + ylen);
  uint8_t* buf = (uint8_t*)malloc(buflen);

  memcpy(buf, x, xlen);
  memcpy(buf + xlen, y, ylen);
  calc_sha_256(out, buf, buflen);

  free(buf);
}