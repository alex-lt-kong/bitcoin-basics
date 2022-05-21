#include <assert.h>
#include <boost/multiprecision/cpp_int.hpp>

#include "utils.h"

using namespace boost::multiprecision;
using namespace std;

int512_t get_int512_from_bytes(const unsigned char* input_bytes, const size_t input_len, const bool bytes_in_big_endian) {

  int512_t result = 0;
  
  assert ((size_t)input_len <= sizeof(int512_t));
  assert (CHAR_BIT == 8); 

  if (bytes_in_big_endian == false) {
    for (int i = input_len; i >= 0; i--) {
      result = (result << 8) + input_bytes[i];
    }
  } else {
    for (int i = 0; i < input_len; i++) {
      result = (result << 8) + input_bytes[i];      
    }
  }
  return result;
}