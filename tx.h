#ifndef TX_H
#define TX_H

#include <stdint.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

using namespace std;

class Tx {
private:
  uint32_t version = 0;
  size_t tx_in_count = 0;
  uint8_t** tx_ins = nullptr;
  uint8_t** tx_outs = nullptr;
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
  Tx(int version, uint8_t** tx_ins, uint8_t** tx_outs, unsigned int locktime, bool is_testnet);
  Tx();
  /**
   * @brief Fill in the Tx instance by parsing bytes from a stringstream.
  */
  void parse(stringstream* ss);
  uint32_t get_version();
  uint32_t get_tx_in_count();
  void to_string();
  /**
   * @brief 
   * @param serialization the memory will NOT be managed by the method.
  */
  static uint32_t parse(uint8_t* serialization);
  ~Tx();
};

class TxIn {
private:
  Tx prev_tx;
  void* prev_index;
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
  TxIn(Tx prev_tx, void* prev_index, void* script_sig, uint32_t sequence);
  
  ~TxIn();
};

#endif