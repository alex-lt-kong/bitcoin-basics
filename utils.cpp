#include <assert.h>
#include <boost/multiprecision/cpp_int.hpp>

#include "utils.h"

using namespace boost;
using namespace CryptoPP;
using namespace std;

multiprecision::int512_t getInt512FromBytes(const unsigned char* inputBytes, short int inputLen, bool bytesInBigEndian) {

  multiprecision::int512_t result = 0;
  
  assert ((size_t)inputLen <= sizeof(multiprecision::int512_t));
  assert (CHAR_BIT == 8); 

  if (bytesInBigEndian == false) {
    for (int i = inputLen; i >= 0; i--) {
      result = (result << 8) + inputBytes[i];
    }
  } else {
    for (int i = 0; i < inputLen; i++) {
      result = (result << 8) + inputBytes[i];      
    }
  }
  return result;
}

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