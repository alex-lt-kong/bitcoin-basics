#ifndef TX_H
#define TX_H

#include <stdint.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "./cryptographic-algorithms/src/sha256.h"
#include "utils.h"

using namespace std;

class Tx;

class TxOut {
private:
  uint64_t amount;
  void* script_pubkey;
protected:
public:
  
  /**
   * @brief Constructor
   * @param version
   * @param tx_ins the memory will be managed by this Tx instance after being passed to it
   * @param tx_outs the memory will be managed by this Tx instance after being passed to it
   * @param locktime
   * @param is_testnet
   */
  TxOut(const uint64_t amount, void* script_pubkey);
  TxOut();
  /**
   * @brief Fill in the TxOut instance by parsing bytes from a stringstream.
   */
  void parse(stringstream* ss);
  uint8_t* serialize();
  uint64_t get_amount();

  ~TxOut();
};

class TxIn {
private:
  uint8_t prev_tx_id[SHA256_HASH_SIZE];
  uint32_t prev_tx_idx;
  void* script_sig;
  uint32_t sequence;
protected:
public:
  
  /**
   * @brief Constructor
   * @param version
   * @param tx_ins the memory will be managed by this Tx instance after being passed to it
   * @param tx_outs the memory will be managed by this Tx instance after being passed to it
   * @param locktime
   * @param is_testnet
   */
  TxIn(const uint8_t* prev_tx_id, uint32_t prev_tx_idx, void* script_sig, uint32_t sequence);
  TxIn();
  /**
   * @brief Fill in the TxIn instance by parsing bytes from a stringstream.
   */
  void parse(stringstream* ss);
  /**
   * @brief get the ID of the previous transaction
   */
  uint8_t* get_prev_tx_id();
  /**
   * @brief get the index of the previous transaction 
   * (i.e., the transaction that specified by the previous transaction's ID)
   */
  uint32_t get_prev_tx_idx();
  uint32_t get_sequence();
  /**
   * @brief Get the output value by looking up the Tx hash.
   * @returns the amount in Satoshi.
   */
  uint64_t get_value(const bool testnet);
  ~TxIn();
};

class Tx {
private:
  uint32_t version = 0;
  size_t tx_in_count = 0;
  size_t tx_out_count = 0;
  vector<TxIn> tx_ins;
  vector<TxOut> tx_outs;
  uint32_t locktime = -1;
  // Used to store the stringstream returned by cURL. As cURL's response is delivered through a callback method,
  // we need to save it to a variable and then read it from another method.
  stringstream* fetched_ss;
  bool is_testnet = false;

  static size_t fetch_tx_cb(char *ptr, size_t size, size_t nmemb, void *This);
protected:
public:
  /**
   * @brief Constructor
   * @param version
   * @param tx_ins the memory will be managed by this Tx instance after being passed to it
   * @param tx_outs the memory will be managed by this Tx instance after being passed to it
   * @param locktime
   * @param is_testnet
   */
  Tx(int version, vector<TxIn> tx_ins, vector<TxOut> tx_outs, unsigned int locktime, bool is_testnet);
  Tx();
  /**
   * @brief Fill in the Tx instance by parsing bytes from a stringstream.
   * @returns whether the stringstream is parsed successfully
   */
  bool parse(stringstream* ss);
  /**
   * @brief Fill in the Tx instance by fetching bytes from a remote URL.
   * @returns whether the fetch()ing and subsequent parse()ing is successful
   */
  bool fetch(const uint8_t tx_id[SHA256_HASH_SIZE], const bool testnet);
  uint32_t get_version();
  uint32_t get_tx_in_count();
  uint32_t get_tx_out_count();
  vector<TxIn> get_tx_ins();
  vector<TxOut> get_tx_outs();
  uint32_t get_locktime();
  uint32_t get_fee();

  void to_string();
  /**
   * @brief 
   * @param serialization the memory will NOT be managed by the method.
   */
  static uint32_t parse(uint8_t* serialization);
  ~Tx();
};


#endif