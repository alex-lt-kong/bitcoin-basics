#include "tx.h"

Tx::Tx(int version, vector<TxIn> tx_ins, void* tx_outs, unsigned int locktime, bool is_testnet) {
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
  ss->read(buf, 4);
  this->version = (buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
  this->tx_in_count = read_variable_int(ss);
  for (size_t i = 0; i < this->tx_in_count; ++i) {
    TxIn tx_in = TxIn();
    tx_in.parse(ss);
    this->tx_ins.push_back(tx_in);
  }
}

uint32_t Tx::get_version() {
  return this->version;
}

uint32_t Tx::get_tx_in_count() {
  return this->tx_in_count;
}

vector<TxIn> Tx::get_tx_ins() {
  return this->tx_ins;
}

Tx::~Tx() {}



TxIn::TxIn(const uint8_t* prev_tx_id, uint32_t prev_tx_idx, void* script_sig, uint32_t sequence) {
  memcpy(this->prev_tx_id, prev_tx_id, SHA256_HASH_SIZE);
  this->prev_tx_idx = prev_tx_idx;
  this->script_sig = script_sig;
  this->sequence = sequence;
}

TxIn::TxIn() {}

void TxIn::parse(stringstream* ss) {
  ss->read((char*)this->prev_tx_id, SHA256_HASH_SIZE);
  reverse(this->prev_tx_id, this->prev_tx_id + SHA256_HASH_SIZE);
  uint8_t buf[4];
  ss->read((char*)buf, 4);
  this->prev_tx_idx = (buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
  uint64_t script_len = read_variable_int(ss);
  char* dummy = (char*)malloc(sizeof(char) * script_len);
  ss->read(dummy, script_len); // The parsing of script will be skipped for the time being.
  free(dummy);
  ss->read((char*)buf, 4);
  this->sequence = (buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
  // The sequence field doesn't appears to be useful for Bitcoin's operation now due to security concerns.
}

uint8_t* TxIn::get_prev_tx_id() {
  return this->prev_tx_id;
}

uint32_t TxIn::get_prev_tx_idx() {
  return this->prev_tx_idx;
}

uint32_t TxIn::get_sequence() {
  return this->sequence;
}

TxIn::~TxIn() {
}
