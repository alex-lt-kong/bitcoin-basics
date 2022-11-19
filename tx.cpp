#include "tx.h"

Tx::Tx(int version, uint8_t** tx_ins, uint8_t** tx_outs, unsigned int locktime, bool is_testnet) {
  this->version = version;
  this->tx_ins = tx_ins;
  this->tx_outs = tx_outs;
  this->locktime = locktime;
  this->is_testnet = is_testnet;
}

Tx::Tx() {
}

void Tx::to_string() {
  printf("This is only a dummy one at the moment!");
}

void Tx::parse(stringstream* ss) {
  char buf[4];
  (*ss).read(buf, 4);
  this->version = (buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
  this->tx_in_count = read_variable_int(ss);
}

uint32_t Tx::get_version() {
  return this->version;
}

uint32_t Tx::get_tx_in_count() {
  return this->tx_in_count;
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



TxIn::TxIn(Tx prev_tx, void* prev_index, void* script_sig, uint32_t sequence) {
  this->prev_tx = prev_tx;
  this->prev_index = prev_index;
  this->script_sig = script_sig;
  this->sequence = sequence;
}

TxIn::~TxIn() {
}
