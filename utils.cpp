#include "utils.h"

using namespace std;
using namespace CryptoPP;

string encodeBytesToHex(unsigned char* inputBytes, int inputLen) {

  string encoded;

  HexEncoder encoder;
  encoder.Put(inputBytes, inputLen);
  encoder.MessageEnd();

  word64 size = encoder.MaxRetrievable();
  if (size) {
      encoded.resize(size);		
      encoder.Get((unsigned char*)&encoded[0], encoded.size());
  }
  return encoded;
}