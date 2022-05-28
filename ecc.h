#ifndef ECC_H
#define ECC_H

#include <boost/multiprecision/cpp_int.hpp>
#include "sha256.h"

using namespace std;
using namespace boost::multiprecision;

// Represent an integer in a finite field. Given the application of the field in this context, it is defined by:
// ** a prime number as order (i.e., size) of the field
// ** a non-negative integer an element of the field. The integer has to be smaller than order of the field.
class FieldElement {
// Note that FieldElement is a number in a given field, infinity is not a valid
// number in that field so we don't need to have a design which handles infinity
// here. Infinity happens only at FieldElementPoint level.
protected:
  // We make the initial value valid so that we can have an empty default constructor.
  // Note that in Python we prepend underscores, in C++ it is recommended to append underscores as variables
  // start with an underscore or double underscore are reserved for the C++ implementers
  int512_t num_ = 0, prime_ = 2;
public:    
  FieldElement(int512_t num, int512_t prime);
  FieldElement();    
  bool operator==(const FieldElement& other) const;
  bool operator!=(const FieldElement& other) const;
  FieldElement operator+(const FieldElement& other);
  FieldElement operator-(const FieldElement& other);
  FieldElement operator*(const FieldElement& other);
  FieldElement operator*(const int512_t other);
  FieldElement operator/(const FieldElement& other);
  FieldElement power(int512_t exponent);
  string to_string(bool inHex=false);
  int512_t num();
  int512_t prime();
};


// Represent a point consists of two finite field elements, given the context of its application, it is defined by:
// ** x, y: two FieldElements representing the coordinates of the point. Not passing x and y to constructor means the point
// is at infinity
// ** a, b: as defined in elliptic curve's canonical form y^2 = x^3 + ax + b
class FieldElementPoint {

protected:
  // The canonical form of an elliptic curve is y^2 = x^3 + ax + b, thus the following a, b, x, y for a point
  // In the book's implementation, a and b are also FieldElements--this seems necessary--if b is not in the same finite
  // field, the addition between two numbers in different fields seems undefined.
  FieldElement a_;
  FieldElement b_;
  FieldElement x_;
  FieldElement y_;
  // We need to have a default constructor FieldElement() defined to make this work.
  bool infinity_ = false;
public:
  FieldElementPoint(FieldElement x, FieldElement y, FieldElement a, FieldElement b);
  FieldElementPoint(FieldElement a, FieldElement b);
  ~FieldElementPoint();
  bool operator==(const FieldElementPoint& other) const;
  FieldElementPoint operator+(const FieldElementPoint& other);
  FieldElementPoint operator+=(const FieldElementPoint& other);
  FieldElementPoint operator*(const int512_t coef);
  string to_string(bool inHex=false);
  bool infinity();
  FieldElement x();
  FieldElement y();
};


class S256Element : public FieldElement
{
private:
  static const int512_t s256_prime_; // the value is defined in ecc.cpp
public:
  S256Element(int512_t num);
  string to_string();
  int512_t s256_prime();
  S256Element power(const int512_t exponent);
  S256Element sqrt();
};

class Signature {
private:
  int512_t r_ = -1;
  int512_t s_ = -1;
public:
  /*
   * @brief Initialize a Signature object
   * @param r the x-coordinate of a random point R from k * G where G is the generator point and k is a random integer
   * @param s the signature proof from a formula involving 
   *  msg_hash, private_key_bytes, r, k_inverse and the order of the generator point G.
   *  The purpose of having s is that we want to derive a number from the private key and prove we know the private key
   *  by revealing only this number (i.e., s) to other people, keeping private key, well, private.
   */
  Signature(int512_t r, int512_t s);
  string to_string();
  // r the x-coordinate of a random point R from k * G where G is the generator point and k is a random integer
  int512_t r();
  /*
   * the signature proof from a formula involving 
   * msg_hash, private_key_bytes, r, k_inverse and the order of the generator point G.
   * The purpose of having s is that we want to derive a number from the private key and prove we know the private key
   * by revealing only this number (i.e., s) to other people, keeping private key, well, private.
   */
  int512_t s();
  /*
   * @brief Get the DER format serialization of a Signature
   * @param output_len pointer to memory where the method writes the length of the output DER hex string.
   * @returns a hex string representing the Signature in DER format
   */
  unsigned char* get_der_format(size_t* output_len);
};

/*
 * A point at the secp256k1 curve
 */
class S256Point : public FieldElementPoint
{
  /* 
   * Note that it is not easy to make a, b, and order public--they are const members of the class,
   * but some of their member functions are non-const. Calling non-constant member from const member
   * seems to cause issues in C++.
   */
protected:
  // The a as defined in y^2 = x^3 + ax + b. It is a FieldElement constant whose value is 0
  static const S256Element a_;
  // The b as defined in y^2 = x^3 + ax + b. It is a FieldElement constant whose value is 7
  static const S256Element b_;
  // The number of elements (i.e. size) of the finite field used by this S256Point.
  // It is a constant whose value is 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141;
  static const int512_t order_;
public:
  S256Point(S256Element x, S256Element y);
  S256Point();
  /*
   * @brief Interpreting this S256Point instance as a public key, verify if a signature if generated by the private 
   *        counterpart of this public key.
   * @param msg_hash the hash value from double SHA256 hash as a representation of the original, potentially much
   *        longer, message.
   * @param sig the Signature object generated (or, signed, if you wish) from the private key by calling the
   *        ECDSAKey object's sign() method.
   */
  bool verify(int512_t msg_hash, Signature sig);
  int512_t s256_prime();
  S256Point operator+(const S256Point other);
  S256Point operator*(const int512_t coef);
  // Get a as defined in y^2 = x^3 + ax + b. It is a FieldElement constant whose value is 0
  S256Element a();
  // Get b as defined in y^2 = x^3 + ax + b. It is a FieldElement constant whose value is 7
  S256Element b();
  // Get the number of elements (i.e. size) of the finite field used by this S256Point.
  // It is a constant whose value is 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141;
  int512_t order();
  string to_string();
  unsigned char* get_sec_format(const bool compressed);
};

// The constant generator point of secp256k1.
extern S256Point G;


class ECDSAKey {
protected:
  unsigned char* privkey_bytes_ = nullptr;
  //How about unsigned char* privkey_bytes_[SHA256_HASH_SIZE]? Seems it will not be as obvious as this...

  int512_t privkey_int_ = -1;
  /*
   * The public key in an ECDSA key pair. It is generated from G * private_key.
   * As an S256Point object, it has a verify() method to verify if a Signature
   * is generated by sign()
   */ 
  S256Point public_key_;
public:
  /*
   * @brief Initialize an ECDSA private key object by providing a secret key in bytes
   * @param private_key_bytes a pointer pointing to an array of bytes as the private_key_bytes
   * @param private_key_len length of the private_key_bytes in bytes. In the current design it can't be greater than 32.
   */
  ECDSAKey(const unsigned char* private_key_bytes, const size_t private_key_len);
  ~ECDSAKey();
  string to_string();
  /*
   * @brief Generate an ECDSA signature with the private key as defined in this instance
   * @param msg_hash a pointer pointing to an array of bytes as the value from hashing
   *        the original message twice with SHA256 algorithm
   * @param msg_hash_len length of the double SHA256 hash, it should always be equal to SHA256_HASH_SIZE.
   * @returns a Signature object
   */
  Signature sign(unsigned char* msg_hash, size_t msg_hash_len);
  /*
   * @brief Get a deterministic (instead of a random) K for ECDSA signature creation per RFC 6979
   * @param msg_hash an unsigned char pointer pointing to the message in bytes
   * @param msg_hash_len length of the message
   * @return the deterministic K
   */
  int512_t get_deterministic_k(unsigned char* msg_hash, size_t msg_hash_len);
  S256Point public_key();
  
};

#endif
