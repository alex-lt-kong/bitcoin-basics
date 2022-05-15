/*
   hmac_sha256.c
   Originally written by https://github.com/h5p9sl
 */

#include "hmac-sha256.h"
#include "sha256.h"

#include <stdlib.h>
#include <string.h>

#define SHA256_BLOCK_SIZE 64

/* LOCAL FUNCTIONS */

// Concatenate X & Y, return hash.
static void* H(const void* x,
               const size_t xlen,
               const void* y,
               const size_t ylen,
               void* out,
               const size_t outlen);

// Declared in hmac_sha256.h
size_t hmac_sha256(const void* key,
                   const size_t keylen,
                   const void* data,
                   const size_t datalen,
                   void* out,
                   const size_t outlen) {
  uint8_t k[SHA256_BLOCK_SIZE];
  uint8_t k_ipad[SHA256_BLOCK_SIZE];
  uint8_t k_opad[SHA256_BLOCK_SIZE];
  uint8_t ihash[SIZE_OF_SHA_256_HASH];
  uint8_t ohash[SIZE_OF_SHA_256_HASH];
  size_t sz;
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
  H(k_ipad, sizeof(k_ipad), data, datalen, ihash, sizeof(ihash));
  H(k_opad, sizeof(k_opad), ihash, sizeof(ihash), ohash, sizeof(ohash));

  sz = (outlen > SIZE_OF_SHA_256_HASH) ? SIZE_OF_SHA_256_HASH : outlen;
  memcpy(out, ohash, sz);
  return sz;
}

static void* H(const void* x,
               const size_t xlen,
               const void* y,
               const size_t ylen,
               void* out,
               const size_t outlen) {
  size_t buflen = (xlen + ylen);
  uint8_t* buf = (uint8_t*)malloc(buflen);

  memcpy(buf, x, xlen);
  memcpy(buf + xlen, y, ylen);
  calc_sha_256(out, buf, buflen);

  free(buf);
}