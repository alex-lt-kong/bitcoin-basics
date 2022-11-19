#include <arpa/inet.h>
#include <assert.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random.hpp>
#include <sstream>
#include "utils.h"
#include "misc.h"

using namespace boost::multiprecision;
using namespace std;

int512_t get_int512_from_bytes(
  const uint8_t* input_bytes, const size_t input_len, const bool bytes_in_big_endian
) {

  int512_t result = 0;
  
  assert (input_len <= 64);
  // 64 bytes * 8 = 512bit, can't use sizeof(int512_t) here, int512_t's size could be greater than 64 bytes
  assert (CHAR_BIT == 8); 

  if (bytes_in_big_endian == false) {
    for (int i = input_len - 1; i >= 0; i--) {
      result = (result << 8) + input_bytes[i];
    }
  } else {
    for (size_t i = 0; i < input_len; i++) {
      result = (result << 8) + input_bytes[i];      
    }
  }
  return result;
}

void get_bytes_from_int256(const int256_t input_int, const bool bytes_in_big_endian, uint8_t* output_bytes) {

  const size_t INT256_SIZE = 32;
  memcpy(output_bytes, &input_int, INT256_SIZE);
  // htonl(47) == 47 means the CPU is big endian, otherwise little endian
  if ((htonl(47) == 47) != bytes_in_big_endian) {
    // That is, the CPU's endianness is different from the desired endianness
    reverse(output_bytes, output_bytes + INT256_SIZE);      
  }
}

bool fermat_primality_test(const int512_t input, const int iterations) {  
  if (input <= 1) {
    return false;
  }

  boost::random::random_device gen;
  boost::random::uniform_int_distribution<uint512_t> ui(1, (uint512_t)(input - 1));
  int512_t x = -1;
  for (int i = 0; i < iterations; i++) {
    x = (int512_t)ui(gen);

    if (powm(x, input - 1, input) != 1) {
      return false;
    }
  }
  return true;
}

char* encode_bytes_to_base58_string(
  const uint8_t* input_bytes, const size_t input_len, const bool bytes_in_big_endian
) {
  //cout << ceil(input_len * 1.36565823) - (input_len * 1.36565823) << endl;
  size_t output_len = ceil(input_len * 1.36565823) + 1; // +1 for null-termination.
  int zero_count = 0;
  while (zero_count < input_len && input_bytes[zero_count] == 0)
		++zero_count; // This is not strictly needed in the current implementation, but let's keep it anyway...
	/*
	 * How do we get the size in advance? We can consider it this way--In base58 encoding, we use 58 characters to encode
	 * bytes (or bits), how many bits can be represented by one character? It is log(2)58 ≈ 5.8579 bits.
	 * Is it possible for base58 character to present 6 bits? No, 2^6 = 64, that is to say, characters will be exhausted
	 * before 0b11 1111 can be encoded.
	 * Since 1 byte = 8 bits, there are (input_len * 8) bits to be represented. Therefore, we need:
	 * (input_len * 8) / log(2)58 = (input_len * log(2)256) / log(2)58 = input_len * log(58)256 ≈ input_len * 1.36565823
	 */
  int512_t num = get_int512_from_bytes(input_bytes, input_len, bytes_in_big_endian);
	char* buf = (char*)calloc(output_len, 1);
  int idx = output_len - 2; // buf[idx-1] should remain \0 to make the string null-terminated.
  static const char b58_table[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
  while (num > 0) {
    buf[idx--] = b58_table[(char)(num % 58)];
    num /= 58;    
  }

  if (zero_count > 0) { assert (zero_count == idx); }
  while (idx > 0) {
    buf[idx--] = b58_table[0];
  }

  /* This part is a dirty hack--the issue is that I can't predict the length of the output with pinpoint
    accuracy...so here if we calloc() too many memory blocks, we shorten the string by memcpy()ing...
    So that we are sure that we wont get a string whose first byte is null...
  */
  char* buf1;
  if (buf[0] == '\0') {
    buf1 = (char*)calloc(output_len - 1, 1);
    memcpy(buf1, buf+1, output_len - 1);
    free(buf);
    return buf1;
  } else { buf1 = buf; }
  return buf1;
}

char* encode_base58_checksum(const uint8_t* input_bytes, const size_t input_len) {
  // return encode_base58(b + hash256(b)[:4])
  uint8_t hash[SHA256_HASH_SIZE];
  cal_sha256_hash(input_bytes, input_len, hash);
  cal_sha256_hash(hash, SHA256_HASH_SIZE, hash);  
  uint8_t base58_input[input_len + 4];

  memcpy(base58_input, input_bytes, input_len);
  memcpy(base58_input + input_len, hash, 4);

  return encode_bytes_to_base58_string(base58_input, input_len + 4, true);
}

void hash160(const uint8_t* input_bytes, const size_t input_len, uint8_t* hash) {
  uint8_t sha256_hash[SHA256_HASH_SIZE];
  cal_sha256_hash(input_bytes, input_len, sha256_hash);
  cal_rpiemd160_hash(sha256_hash, SHA256_HASH_SIZE, hash);
}

uint64_t read_variable_int(stringstream* ss) {
  // Per C standard, shifting by a negative value or a value greater than or equal to the number of bits of
  // the left operand is undefined. We need to cast the left operand to a bigger type of integer to make it work.
  // I believe prt[1] does not need to be casted, just casting it make the code a bit prettier... 

  uint8_t buf[8] = {0}; // defining buf as char[] seems to trigger UB
  (*ss).read((char*)buf, 1);  
  
  if (buf[0] == 0xfd) {
    // the next two bytes are the number
    (*ss).read((char*)buf, 2);
    return (buf[0] << 0) | (buf[1] << 8);
  } else if (buf[0] == 0xfe) {
    // the next four bytes are the number
    (*ss).read((char*)buf, 4);
    return ((uint64_t)buf[0] << 0) | ((uint64_t)buf[1] << 8) | ((uint64_t)buf[2] << 16) | ((uint64_t)buf[3] << 24);
  } else if (buf[0] == 0xff) {
    // the next eight bytes are the number
    (*ss).read((char*)buf, 8);
    return (
      ((uint64_t)buf[1] << 0)  | ((uint64_t)buf[1] << 8)  | ((uint64_t)buf[2] << 16) | ((uint64_t)buf[3] << 24) |
      ((uint64_t)buf[4] << 32) | ((uint64_t)buf[5] << 40) | ((uint64_t)buf[6] << 48) | ((uint64_t)buf[7] << 56)
    );
  } else {
    return buf[0];
  }
}

uint8_t* encode_variable_int(const uint64_t num, size_t* int_len) {
  uint8_t* result = nullptr;
  if (num < 0xfd) {
    *int_len = 1 + 0;
    result = (uint8_t*)malloc(*int_len);
    memcpy(result + 0, &num, 1);    
  } else if (num < 0x10000) { // 1048576, i.e., 2^20
    *int_len = 1 + 2;
    result = (uint8_t*)malloc(*int_len);
    result[0] = 0xfd;
    memcpy(result + 1, &num, 2);
  } else if (num < 0x100000000) { // 4294967296, i.e., 2^32
    *int_len = 1 + 4;
    result = (uint8_t*)malloc(*int_len);
    result[0] = 0xfe;
    memcpy(result + 1, &num, 4);
  } else {
    *int_len = 1 + 8;
    result = (uint8_t*)malloc(*int_len);
    result[0] = 0xff;
    memcpy(result + 1, &num, 8);
  }
  return result;
}
