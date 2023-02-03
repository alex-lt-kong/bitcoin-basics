#include "tx.h"

Tx::Tx(int version, vector<TxIn> tx_ins, vector<TxOut> tx_outs,
    unsigned int locktime, bool is_testnet) {
    this->version = version;
    this->tx_ins = tx_ins;
    this->tx_outs = tx_outs;
    this->locktime = locktime;
    this->is_testnet = is_testnet;
}

Tx::Tx() {}

bool Tx::parse(vector<uint8_t>& d) {
    if (d.size() < 4) {
        fprintf(stderr, "byte vector doesn't contain expected number of bytes. "
            "\nExpect: 4\nActual: %lu\nTx::parse() failed and the Tx instance "
            "is corrupted\n", d.size());
        return false;
    }
    version = (d[0] << 0 | d[1] << 8 | d[2] << 16 | d[3] << 24);
    d.erase(d.begin(), d.begin() + 4);
    tx_in_count = read_variable_int(d);
    for (size_t i = 0; i < this->tx_in_count; ++i) {
        TxIn tx_in = TxIn();
        tx_in.parse(d);
        this->tx_ins.push_back(tx_in);
    }
    tx_out_count = read_variable_int(d);
    for (size_t i = 0; i < tx_out_count; ++i) {
        TxOut tx_out = TxOut();
        tx_out.parse(d);
        tx_outs.push_back(tx_out);
    }
    if (d.size() == 4) {        
        locktime = (d[0] << 0 | d[1] << 8 | d[2] << 16 | d[3] << 24);
        d.erase(d.begin(), d.begin() + 4);
    } else {
        fprintf(stderr, "byte vector doesn't contain expected number of bytes. "
            "\nExpect: 4\nActual: %lu\nTx::parse() failed and the Tx instance "
            "is corrupted\n", d.size());
        return false;
    }
    return true;
}


int Tx::fetch_tx(const uint8_t tx_id[SHA256_HASH_SIZE], vector<uint8_t>& d) {
    char* tx_id_str = bytes_to_hex_string(tx_id, SHA256_HASH_SIZE, false);
    if (tx_id_str == NULL) {
        fprintf(stderr, "Failed to get tx_id string.\n");    
        return 1;
    }
    if (strlen(tx_id_str) != SHA256_HASH_SIZE * 2) {
        fprintf(stderr, "Invalid tx_id string length: %lu\n", strlen(tx_id_str));
        free(tx_id_str);
        return 2;
    }
    char* tx_id_hex = bytes_to_hex_string(tx_id, SHA256_HASH_SIZE, false);
    json data = bitcoind_rpc(
        R"({"jsonrpc": "1.0", "method": "getrawtransaction", "params": [")" +
        string(tx_id_hex) + "\"]}");
    free(tx_id_hex);
    if (data["result"].is_null()) {
        fprintf(stderr, "bitcoind_rpc() failed: %s\n", data.dump().c_str());
        return 3;
    }
    string tx_hex = data["result"].get<string>();
    d.resize(tx_hex.size() + 1);
    memcpy(d.data(), tx_hex.data(), tx_hex.size());
    d[tx_hex.size()] = '\0';
    return 0;
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
        input_sum += this->tx_ins[i].get_value();
    }
    for (size_t i = 0; i < this->tx_out_count; ++i) {
        output_sum += this->tx_outs[i].get_amount();
    }
    return input_sum - output_sum;
}

Tx::~Tx() {}



TxIn::TxIn(const uint8_t* prev_tx_id, uint32_t prev_tx_idx,
    void* script_sig, uint32_t sequence) {
    memcpy(this->prev_tx_id, prev_tx_id, SHA256_HASH_SIZE);
    this->prev_tx_idx = prev_tx_idx;
    this->script_sig = script_sig;
    this->sequence = sequence;
}

TxIn::TxIn() {}

void TxIn::parse(vector<uint8_t>& d) {
    memcpy(this->prev_tx_id, d.data(), SHA256_HASH_SIZE);
    d.erase(d.begin(), d.begin() + SHA256_HASH_SIZE);
    
    reverse(this->prev_tx_id, this->prev_tx_id + SHA256_HASH_SIZE);
    
    this->prev_tx_idx = (d[0] << 0 | d[1] << 8 | d[2] << 16 | d[3] << 24);
    d.erase(d.begin(), d.begin() + 4);
    uint64_t script_len = read_variable_int(d);
    d.erase(d.begin(), d.begin() + script_len);
    // The parsing of script will be skipped for the time being.

    this->sequence = (d[0] << 0 | d[1] << 8 | d[2] << 16 | d[3] << 24);
    d.erase(d.begin(), d.begin() + 4);
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

uint64_t TxIn::get_value() {
    Tx tx = Tx();
    vector<uint8_t> d(64);
    if (Tx::fetch_tx(this->get_prev_tx_id(), d) != 0) {
        fprintf(stderr, "Failed to Tx::fetch() tx_id\n");
        return 0;
    }
    int64_t hex_len;
    unique_byte_ptr hex_input(hex_string_to_bytes((char*)d.data(), &hex_len));
    d.resize(hex_len); // let's re-use the same vector...
    memcpy(d.data(), hex_input.get(), hex_len);
    tx.parse(d);
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

void TxOut::parse(vector<uint8_t>& d) {
    uint8_t buf[8];
    memcpy(buf, d.data(), 8);
    d.erase(d.begin(), d.begin() + 8);
    this->amount = (
        buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24
    );
    uint64_t script_len = read_variable_int(d);

    d.erase(d.begin(), d.begin() + script_len);
    // The parsing of script will be skipped for the time being.
}

uint64_t TxOut::get_amount() {
    return this->amount;
}

uint8_t* serialize() {
    return (uint8_t*)nullptr;
}

TxOut::~TxOut() {
}
