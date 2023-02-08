#include "tx.h"

Tx::Tx() {}


bool Tx::parse(vector<uint8_t>& d) {
    // https://en.bitcoin.it/wiki/Protocol_documentation#tx
    if (d.size() < 60) {
        cerr << "Tx::parse() failed: byte vector doesn't contain "
             << "expected number of bytes. \n"
             << "Expect: >= 60\n"
             << "Actual: " << d.size() << "\n"
             << "the Tx instance is corrupted\n";
        return false;
    }
    version = (d[0] << 0 | d[1] << 8 | d[2] << 16 | d[3] << 24);
    d.erase(d.begin(), d.begin() + 4);

    if (d[0] == 0 && d[1] == 1) {
        // BIP141, first block with witness_flag enabled: 481824
        witness_flag = true;
        d.erase(d.begin(), d.begin() + 2);
    }

    read_variable_int(d, &tx_in_count);
    for (size_t i = 0; i < tx_in_count; ++i) {
        TxIn tx_in = TxIn(d);
        tx_ins.push_back(tx_in);
    }
    read_variable_int(d, &tx_out_count);
    for (size_t i = 0; i < tx_out_count; ++i) {
        TxOut tx_out = TxOut(d);
        tx_outs.push_back(tx_out);
    }

    if (witness_flag) {
        if (d.size() < tx_in_count) {
            cerr << "Tx::parse() failed: byte vector doesn't contain "
                << "expected number of bytes. \n"
                << "Expect: >= " << tx_in_count << "\n"
                << "Actual: " << d.size() << "\n"
                << "the Tx instance is corrupted\n";
            return false;
        }
        for (size_t i = 0; i < tx_in_count; ++i) {
            size_t witeness_count;
            read_variable_int(d, &witeness_count);
            tx_ins[i].witenesses = vector<vector<uint8_t>>(witeness_count);
            if (d.size() < witeness_count * 2) {
                // each witness needs one single-digit varint + a byte of data
                cerr << "Tx::parse() failed: byte vector doesn't contain "
                    << "expected number of bytes. \n"
                    << "Expect: >= " << witeness_count * 2 << "\n"
                    << "Actual: " << d.size() << "\n"
                    << "the Tx instance is corrupted\n";
                return false;
            }
            for (size_t j = 0; j < witeness_count; ++j) {
                size_t witeness_size;
                read_variable_int(d, &witeness_size);
                tx_ins[i].witenesses[j] = vector<uint8_t>(witeness_size);
                memcpy(tx_ins[i].witenesses[j].data(), d.data(), witeness_size);
                d.erase(d.begin(), d.begin() + witeness_size);
            }
        }
    }

    if (d.size() == 4) {        
        locktime = (d[0] << 0 | d[1] << 8 | d[2] << 16 | d[3] << 24);
        d.erase(d.begin(), d.begin() + 4);
    } else {
        cerr << "Tx::parse() failed: byte vector doesn't contain "
             << "expected number of bytes.\n"
             << "Expect: 4\n"
             << "Actual: " << d.size() << "\nThe Tx instance is corrupted"
             << endl;
        return false;
    }
    return true;
}


int Tx::fetch_tx(const uint8_t tx_id[SHA256_HASH_SIZE], vector<uint8_t>& d) {
    unique_char_ptr tx_id_hex(bytes_to_hex_string(
        tx_id, SHA256_HASH_SIZE, false));
    json data = bitcoind_rpc(
        R"({
            "jsonrpc": "1.0",
            "method": "getrawtransaction",
            "params": [")" + string(tx_id_hex.get()) + R"("]
        })");
    if (data["result"].is_null()) {
        cerr << "bitcoind_rpc() failed: " << data.dump().c_str() << endl;
        return 3;
    }
    string tx_hex = data["result"].get<string>();
    d.resize(tx_hex.size() + 1);
    memcpy(d.data(), tx_hex.data(), tx_hex.size());
    d[tx_hex.size()] = '\0';
    return 0;
}

uint32_t Tx::get_version() {
    return version;
}

uint32_t Tx::get_tx_in_count() {
    return tx_in_count;
}

uint32_t Tx::get_tx_out_count() {
    return tx_out_count;
}

vector<TxIn> Tx::get_tx_ins() {
    return tx_ins;
}

vector<TxOut> Tx::get_tx_outs() {
    return tx_outs;
}

uint32_t Tx::get_locktime() {
    return locktime;
}

uint32_t Tx::get_fee() {
    uint32_t input_sum = 0, output_sum = 0;
    for (size_t i = 0; i < tx_in_count; ++i) {
        input_sum += tx_ins[i].get_value();
    }
    for (size_t i = 0; i < tx_out_count; ++i) {
        output_sum += tx_outs[i].get_amount();
    }
    return input_sum - output_sum;
}

Tx::~Tx() {}

TxIn::TxIn() {}

TxIn::TxIn(vector<uint8_t>& d) {
    if (d.size() < SHA256_HASH_SIZE) {
        throw invalid_argument("byte vector doesn't contain "
            "expected number of bytes. Expect: " + to_string(SHA256_HASH_SIZE) +
            ", actual: " + to_string(d.size()) + ".");
    }
    memcpy(prev_tx_id, d.data(), SHA256_HASH_SIZE);
    d.erase(d.begin(), d.begin() + SHA256_HASH_SIZE);
    
    reverse(prev_tx_id, prev_tx_id + SHA256_HASH_SIZE);
    if (d.size() < 4) {
        throw invalid_argument("byte vector doesn't contain expected number of "
            "bytes. Expect: 4, actual: " + to_string(d.size()) + ".");
    }
    prev_tx_idx = (d[0] << 0 | d[1] << 8 | d[2] << 16 | d[3] << 24);
    d.erase(d.begin(), d.begin() + 4);
    uint64_t script_len;
    read_variable_int(d, &script_len);
    d.erase(d.begin(), d.begin() + script_len);
    // The parsing of script will be skipped for the time being.

    if (d.size() < 4) {
        throw invalid_argument("byte vector doesn't contain expected number of "
            "bytes. \nExpect: 4\nActual: " + to_string(d.size()) + ".");
    }
    sequence = (d[0] << 0 | d[1] << 8 | d[2] << 16 | d[3] << 24);
    d.erase(d.begin(), d.begin() + 4);
}

uint8_t* TxIn::get_prev_tx_id() {
    return prev_tx_id;
}

uint32_t TxIn::get_prev_tx_idx() {
    return prev_tx_idx;
}

uint32_t TxIn::get_sequence() {
    return sequence;
}

uint64_t TxIn::get_value() {
    Tx tx = Tx();
    vector<uint8_t> d(64);
    if (Tx::fetch_tx(get_prev_tx_id(), d) != 0) {
        cerr << "Failed to Tx::fetch() tx_id\n";
        return 0;
    }
    int64_t hex_len;
    unique_byte_ptr hex_input(hex_string_to_bytes((char*)d.data(), &hex_len));
    d.resize(hex_len); // let's re-use the same vector...
    memcpy(d.data(), hex_input.get(), hex_len);
    tx.parse(d);
    vector<TxOut> tx_outs = tx.get_tx_outs();
    return tx_outs[get_prev_tx_idx()].get_amount();
}

TxIn::~TxIn() {
}

TxOut::TxOut() {}

TxOut::TxOut(vector<uint8_t>& d) {
    uint8_t buf[8];
    memcpy(buf, d.data(), 8);
    d.erase(d.begin(), d.begin() + 8);
    amount = (
        buf[0] << 0 | buf[1] << 8 | buf[2] << 16 | buf[3] << 24
    );
    uint64_t script_len;
    read_variable_int(d, &script_len);

    d.erase(d.begin(), d.begin() + script_len);
    // The parsing of script will be skipped for the time being.
}

uint64_t TxOut::get_amount() {
    return amount;
}

uint8_t* serialize() {
    return (uint8_t*)nullptr;
}

TxOut::~TxOut() {
}
