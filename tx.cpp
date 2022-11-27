#include <curl/curl.h>

#include "tx.h"

Tx::Tx(int version, vector<TxIn> tx_ins, vector<TxOut> tx_outs, unsigned int locktime, bool is_testnet) {
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

bool Tx::parse(stringstream* ss) {
  char buf[4];
  ss->read(buf, 4);
  this->version = (buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
  this->tx_in_count = read_variable_int(ss);
  for (size_t i = 0; i < this->tx_in_count; ++i) {
    TxIn tx_in = TxIn();
    tx_in.parse(ss);
    this->tx_ins.push_back(tx_in);
  }
  this->tx_out_count = read_variable_int(ss);
  for (size_t i = 0; i < this->tx_out_count; ++i) {
    TxOut tx_out = TxOut();
    tx_out.parse(ss);
    this->tx_outs.push_back(tx_out);
  }
  ss->read(buf, 4);
  this->locktime = (buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
  return true;
}

size_t Tx::fetch_tx_cb(char *ptr, size_t size, size_t nmemb, void *This) {
  // The method is designed this way following the instructions here:
  // https://daniel.haxx.se/blog/2021/09/27/common-mistakes-when-using-libcurl/
  size_t realsize = size * nmemb;
  /*
  for (int i = 0; i < realsize; ++i) {
    printf("%c", ptr[i]);
  }
  printf("\n");
  */
  uint8_t* null_terminated_resp = (uint8_t*)calloc(realsize + 1, sizeof(uint8_t));
  if (null_terminated_resp == NULL) {
    return 0;
  }
  memcpy(null_terminated_resp, ptr, realsize);
  size_t out_len;
  char* hex_input = (char*)hex_string_to_bytes((char*)null_terminated_resp, &out_len);
  ((Tx*)This)->fetched_ss->write(hex_input, realsize);
  free(hex_input);
  return realsize;
}

bool Tx::fetch(const uint8_t tx_id[SHA256_HASH_SIZE], const bool testnet) {
  char* tx_id_bytes = bytes_to_hex_string(tx_id, SHA256_HASH_SIZE, false);
  char url[PATH_MAX] = {0};
  if (testnet) {
    snprintf(url, PATH_MAX, "https://blockstream.info/testnet/api/tx/%s/hex", tx_id_bytes);
  } else {
    snprintf(url, PATH_MAX, "https://blockstream.info/api/tx/%s/hex", tx_id_bytes);
  }  
  free(tx_id_bytes);
  CURL *curl;
  CURLcode res;
 
  curl = curl_easy_init();
  stringstream _ss = stringstream("");
  this->fetched_ss = &_ss;
  bool retval = false;
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Tx::fetch_tx_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    else {
       retval = this->parse(this->fetched_ss);
    }
    /* always cleanup */
    curl_easy_cleanup(curl);    
  }
  return retval;
}

uint32_t Tx::get_version() {
  return this->version;
}

uint32_t Tx::get_tx_in_count() {
  return this->tx_in_count;
}

uint32_t Tx::get_tx_out_count() {
  return this->tx_out_count;
}

vector<TxIn> Tx::get_tx_ins() {
  return this->tx_ins;
}

vector<TxOut> Tx::get_tx_outs() {
  return this->tx_outs;
}

uint32_t Tx::get_locktime() {
  return this->locktime;
}

uint32_t Tx::get_fee() {
  uint32_t input_sum = 0, output_sum = 0;
  for (size_t i = 0; i < this->tx_in_count; ++i) {
    input_sum += this->tx_ins[i].get_value(false);
  }
  for (size_t i = 0; i < this->tx_out_count; ++i) {
    output_sum += this->tx_outs[i].get_amount();
  }
  return input_sum - output_sum;
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

uint64_t TxIn::get_value(const bool testnet) {
  Tx tx = Tx();
  tx.fetch(this->get_prev_tx_id(), testnet);
  vector<TxOut> tx_outs = tx.get_tx_outs();
  return tx_outs[this->get_prev_tx_idx()].get_amount();
}

TxIn::~TxIn() {
}




TxOut::TxOut(const uint64_t amount, void* script_pubkey) {
  this->amount = amount;
  this->script_pubkey = script_pubkey;
}

TxOut::TxOut() {}

void TxOut::parse(stringstream* ss) {
  uint8_t buf[8];
  ss->read((char*)buf, 8);
  this->amount = (
    buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24
  );
  uint64_t script_len = read_variable_int(ss);
  char* dummy = (char*)malloc(sizeof(char) * script_len);
  ss->read(dummy, script_len); // The parsing of script will be skipped for the time being.
  free(dummy);
}

uint64_t TxOut::get_amount() {
  return this->amount;
}

uint8_t* serialize() {
  return (uint8_t*)nullptr;
}

TxOut::~TxOut() {
}
