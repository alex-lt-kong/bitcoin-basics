#ifndef TX_H
#define TX_H

#include <stdint.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "./cryptographic-algorithms/sha256.h"
#include "utils.h"

using namespace std;

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
  void parse(stringstream* ss);
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
  void parse(stringstream* ss);
  uint8_t* get_prev_tx_id();
  uint32_t get_prev_tx_idx();
  uint32_t get_sequence();

  ~TxIn();
};

class Tx {
private:
  uint32_t version = 0;
  size_t tx_in_count = 0;
  size_t tx_out_count = 0;
  vector<TxIn> tx_ins;
  vector<TxOut> tx_outs;
  unsigned int locktime = -1;
  bool is_testnet = false;
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
  */
  void parse(stringstream* ss);
  uint32_t get_version();
  uint32_t get_tx_in_count();
  uint32_t get_tx_out_count();
  vector<TxIn> get_tx_ins();
  vector<TxOut> get_tx_outs();
  void to_string();
  /**
   * @brief 
   * @param serialization the memory will NOT be managed by the method.
  */
  static uint32_t parse(uint8_t* serialization);
  ~Tx();
};


#endif