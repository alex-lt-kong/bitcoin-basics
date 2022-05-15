#ifndef UTILS_H
#define UTILS_H

#include <boost/multiprecision/cpp_int.hpp>
#include <crypto++/sha.h>
#include <crypto++/hex.h>

using namespace boost;
using namespace std;
using namespace CryptoPP;

// The most confusing parameter is the 3rd one, bytesInBigEndian.
// If set to true, inputBytes stores the most significant byte of a word at the smallest memory address.
// Suppose we store 65,280 in two bytes, if bytes are 0xff 0x00, then inputBytes is in big-endian order
// because the most significant byte is at the beginning of the array, which has the smallest memory address.
// What if inputBytes is still { 0xff, 0x00 } but we set bytesInBigEndian = false? The int512_t should be 255.
multiprecision::int512_t getInt512FromBytes(const unsigned char* inputBytes, short int inputLen, bool bytesInBigEndian=true);

string encodeBytesToHex(unsigned char* inputBytes, int inputLen);

#endif