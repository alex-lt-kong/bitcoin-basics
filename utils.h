#ifndef UTILS_H
#define UTILS_H

#include <boost/multiprecision/cpp_int.hpp>
#include "sha256.h"
#include "ripemd160.h"

using namespace boost::multiprecision;
using namespace std;

/**
 * @brief Convert a byte array to an int512_t integer
 * @param input_bytes The byte array to be converted to int512_t
 * @param input_len length of input_bytes
 * @param bytes_in_big_endian whether input_bytes is in little or big endian order
 * @retval an int512_t variable from input_bytes
 * 
 * @note The most confusing parameter is the 3rd one, bytes_in_big_endian.
 * If set to true, input_bytes stores the most significant byte of a word at the smallest memory address.
 * Suppose we store 65,280 in two bytes, if bytes are 0xff 0x00, then input_bytes is in big-endian order
 * because the most significant byte is at the beginning of the array, which has the smallest memory address.
 * What if input_bytes is still { 0xff, 0x00 } but we set bytes_in_big_endian = false? The int512_t should be 255.
 */
int512_t get_int512_from_bytes(
  const unsigned char* input_bytes, const size_t input_len, const bool bytes_in_big_endian=true
);

/**
 * @brief Convert an int256_t integer to a byte array
 * @param input_int The int256_t variable to be convereted to a byte array
 * @param bytes_in_big_endian whether the output_bytes should in little or big endian order
 * @param output_bytes a pre-allocated, 32-byte long array used to store the result
 */
void get_bytes_from_int256(
  const int256_t input_int, const bool bytes_in_big_endian, unsigned char* output_bytes
);

/**
 *  @brief Test if the input is probably a prime number by applying Fermat's little theorem
 *  @param input the number to be checked
 *  @param iterations the number of times the test should be done.
 */
bool fermat_primality_test(const int512_t input, const int iterations);

/**
 * @brief Encode a byte array into a base58 string
 * @param input_bytes pointer to data in byte array to be encoded
 * @param input_len Length of the data to be encoded
 * @param bytes_in_big_endian whether input_bytes is in little or big endian order
 * @returns Pointer to a null-terminated string storing the base58 representation of the input byte array.
 * Users need to free() the pointer after use.
 */
char* encode_bytes_to_base58_string(
  const unsigned char* input_bytes, const size_t input_len, const bool bytes_in_big_endian
);

/**
 * @returns Pointer to a null-terminated string. Users need to free() the pointer after use.
*/
char* encode_base58_checksum(const unsigned char* input_bytes, const size_t input_len);

/**
 * @brief Calculate the hash160 hash value (i.e., RIPEMD160 on top of SHA256) from a given byte array
 * @param input_bytes Pointer to the data the hash shall be calculated on.
 * @param input_len Length of the input_bytes data, in byte.
 * @param hash Preallocated 20-byte long array, where the result is delivered.
*/
void hash160(const unsigned char* input_bytes, const size_t input_len, unsigned char* hash);
#endif