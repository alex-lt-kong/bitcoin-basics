#ifndef ECC_H
#define ECC_H

#include <boost/multiprecision/cpp_int.hpp>
#include <mycrypto/sha256.h>
#include <mycrypto/hmac.h>
#include <mycrypto/misc.h>

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
  /**
   * @brief Initialize an element of a finite field
   * @param num the number of such element
   * @param prime the order of the finite field
   */
  FieldElement(int512_t num, int512_t prime);
  /**
   * @brief A dummy constructor needed by C++'s syntax. Directly calling it may 
   * cause undefined behaviors.
   */
  FieldElement();    
  bool operator==(const FieldElement& other) const;
  bool operator!=(const FieldElement& other) const;
  FieldElement operator+(const FieldElement& other);
  FieldElement operator-(const FieldElement& other);
  FieldElement operator*(const FieldElement& other);
  FieldElement operator*(const int512_t other);
  FieldElement operator/(const FieldElement& other);
  /**
   * @brief Apply the exponentiation operation to the FieldElement object, using the object as base
   * @param exponent
   * @return 
  */
  FieldElement power(int512_t exponent);
  string to_string(bool inHex=false);
  /**
   * @brief Get the number of the element. For the purpose of Bitcoin application, the number must be:
   * 1. At most 512-bit long (in contrast, the long long type in C is 64-bit long)
   * 2. A non-negative number smaller than the order (i.e. pre-defined prime) of the finite field   * 
   * @return the number of the field element 
   */
  int512_t num();
  int512_t prime();
};


// Represent a point consists of two finite field elements, given the context of its application, it is defined by:
// ** x, y: two FieldElements representing the coordinates of the point. Not passing x and y to constructor means the point
// is at infinity
// ** a, b: as defined in elliptic curve's canonical form y^2 = x^3 + ax + b
/**
 * @brief Represent a point on anelliptic curve.
 * The point consists of two finite field elements, given the context of its application, it is defined by:
 * x, y: two FieldElements representing the coordinates of the point or, in absence of them, a point at infinity
 * a, b: as defined in elliptic curve's canonical form y^2 = x^3 + ax + b
*/
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
  /**
   * @brief Initialize a field element point on curve y^2 = x^3 + ax + b
   * @param x the x coordinate of the point
   * @param y the y coordinate of the point
   * @param a the a coefficient as defined in the elliptic curve's canonical form
   * @param b the b coefficient as defined in the elliptic curve's canonical form
   * @throw invalid_argument if a, b, x, y are not in the same finite field (i.e., their prime numbers are different)
   * or if the point (x, y) is not on the curve
   */
  FieldElementPoint(FieldElement x, FieldElement y, FieldElement a, FieldElement b);
  /**
   * @brief Initialize a field element point at infinity "on" curve y^2 = x^3 + ax + b. No x and y are passed.
   * @param a the a coefficient as defined in the elliptic curve's canonical form
   * @param b the b coefficient as defined in the elliptic curve's canonical form
   * @throw invalid_argument if a and b are not in the same finite field (i.e., their prime numbers are different)
   */
  FieldElementPoint(FieldElement a, FieldElement b);
  ~FieldElementPoint();
  bool operator==(const FieldElementPoint& other) const;
  FieldElementPoint operator+(const FieldElementPoint& other);
  FieldElementPoint operator+=(const FieldElementPoint& other);
  FieldElementPoint operator*(const int512_t coef);
  string to_string(bool inHex=false);
  /**
   * @brief Check if the FieldElementPoint is at Infinity
   */
  bool infinity();
  /**
   * @brief Get the x coordinate of the point 
   * @return FieldElement 
   * @throw invalid_argument if the point is at infinity
   */
  FieldElement x();
  /**
   * @brief Get the y coordinate of the point 
   * @return FieldElement 
   * @throw invalid_argument if the point is at infinity
   */
  FieldElement y();
  FieldElement a();
  FieldElement b();
};

/**
 * @brief Field element specific to the curve Secp256k1 (y^2 = x^3 + ax + b). Its order is hardcoded to be
 * 0xFFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFC2F or
 * 2^256 - 2^32 - 2^9 - 2^8 - 2^7 - 2^6 - 2^4 - 1
 */
class S256Element : public FieldElement
{
private:
  static const int512_t s256_prime_; // the value is defined in ecc.cpp
public:
  /**
   * @brief Initialize a S256Element by passing an integer. Order of the finite field is not needed as it is hardcoded
   * to be 0xFFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFC2F or
   * 2^256 - 2^32 - 2^9 - 2^8 - 2^7 - 2^6 - 2^4 - 1
   * @param num the integer of the finite field element
   */
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
  /**
   * @brief Initialize a Signature object
   * @param r the x-coordinate of a random point R from k * G where G is the generator point and k is a deterministic
   * "random" integer generated by the algorithm defined in RFC6979
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
  uint8_t* get_der_format(size_t* output_len);
};

/**
 * @brief A field element point on the secp256k1 curve (y^2 = x^3 + 7)
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
  /**
   * @brief Initialize a field element point on the secp256k1 curve (y^2 = x^3 + 7). To initialize a point at infinity,
   * omit the x and y coordiates.
   * @param x the x coordinate of the point
   * @param y the y coordinate of the point   
   */
  S256Point(S256Element x, S256Element y);
  /**
   * @brief Initialize a field element point at infinity on the secp256k1 curve (y^2 = x^3 + 7).
   */
  S256Point();
  /**
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
  /**
   * @brief Get an S256Point's representation in Standards for Efficient Cryptography (SEC) format
   * @param compressed Should the SEC's uncompressed or compressed version be returned. Compressed SEC format
   * basically store the x coordinate of the point only--the y coordinate can be derived from the x coordinate.
   * Therefore we save almost 50% of the length.
   * @returns Pointer to a byte array storing the S256Point in SEC format.
   * If compression is enabled, the array is 33-byte long; otherwise it is 65-byte long.
   * Users are reminded to free() the pointer after use.
   */
  uint8_t* get_sec_format(const bool compressed);
  /**
   * @brief A detailed comment is not provided because it is still not sure about the purpose of this method...
   * @returns Pointer to a null-terminated string. Users are reminded to free() the pointer after use.
   */
  char* get_address(bool compressed, bool testnet);
};

// The constant generator point of secp256k1.
extern S256Point G;

class ECDSAKey {
protected:
  uint8_t privkey_bytes_[32] = {0};
  int512_t privkey_int_ = -1;
  /**
   * The public key in an ECDSA key pair. It is generated from G * private_key.
   * As an S256Point object, it has a verify() method to verify if a Signature
   * is generated by sign()
   */ 
  S256Point public_key_;
public:
  /**
   * @brief Initialize an ECDSA private key object by providing a secret key in bytes
   * @param private_key_bytes a pointer pointing to an array of bytes as the private_key_bytes
   * @param private_key_len length of the private_key_bytes in bytes. In the current design it can't be greater than 32.
   * @param reverse_byte_order this parameter is only useful if the private_keys_bytes are originally from a number,
   * one needs to set this parameter correspondingly to let the constructor interpret the bytes array correctly.
   * That is, for bit-endian number, set it to false; for little-endian number, set it to true.
   * If the bytes array is given as it is, this can be left not set, which is default to false.
   */
  ECDSAKey(const uint8_t* private_key_bytes, const size_t private_key_len, bool reverse_byte_order = false);
  /**
   * @brief Initialize an ECDSA private key object by providing a secret key in int512_t
   * @param private_key the private key
   */
  ECDSAKey(const int512_t private_key);
  ~ECDSAKey();
  string to_string();
  /**
   * @brief Generate an ECDSA signature with the private key as defined in this instance
   * @param msg_hash a pointer pointing to an array of bytes as the value from hashing
   *        the original message twice with SHA256 algorithm
   * @param msg_hash_len length of the double SHA256 hash, it should always be equal to SHA256_HASH_SIZE.
   * @returns a Signature object
   */
  Signature sign(uint8_t* msg_hash, size_t msg_hash_len);
  /**
   * @brief Get a deterministic (instead of a random) K for ECDSA signature creation per RFC 6979
   * @param msg_hash an uint8_t pointer pointing to the message in bytes
   * @param msg_hash_len length of the message
   * @return the deterministic K
   */
  int512_t get_deterministic_k(uint8_t* msg_hash, size_t msg_hash_len);
  /**
   * @brief Get the public key of this ECDSAKey instance
   */
  S256Point public_key();
  /**
   * @brief Get the private key of this ECDSAKey instance in Wallet Import Format (WIF format)
   * @param compressed If the private key is compressed? (But we are not using SEC, what's the difference?)
   * @param testnet If the key is to be used on the testnet?
   * @returns Pointer to a null-terminated string containing the private key in WIF format.
   * Users are reminded to free() the pointer after use.
   */
  char* get_wif_private_key(bool compressed, bool testnet);
  
};

#endif
