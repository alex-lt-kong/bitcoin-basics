#include <stdio.h>
#include <stdlib.h>

#include "tx.h"

Tx::Tx(int version, uint8_t** tx_ins, uint8_t** tx_outs, unsigned int locktime, bool is_testnet) {
  this->version = version;
  this->tx_ins = tx_ins;
  this->tx_outs = tx_outs;
  this->locktime = locktime;
  this->is_testnet = is_testnet;
}

void Tx::to_string() {
  printf("This is only a dummy one at the moment!");
}

Tx::~Tx() {
  /* This is NOT a proper implementation, note that tx_ins/tx_outs are pointers of pointers!*/
  if (tx_ins != nullptr) {
    free(tx_ins);
  }
  if (tx_outs != nullptr) {
    free(tx_outs);
  }
}

void Tx::parse(uint8_t* serialization) {
  uint8_t* ptr = serialization;
  unsigned int version = (ptr[0] << 0) | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
  printf("%u\n", version);
}