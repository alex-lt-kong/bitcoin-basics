#ifndef UTILS_H
#define UTILS_H

#include "crypto++/sha.h"
#include "crypto++/hex.h"

using namespace std;
using namespace CryptoPP;

void getSHA256hash(unsigned char* inputBytes, int dataLen, unsigned char** outputBytes);
string encodeBytesToHex(unsigned char* inputBytes, int inputLen);

#endif