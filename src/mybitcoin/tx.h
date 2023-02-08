#ifndef TX_H
#define TX_H

#include <stdint.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <mycrypto/sha256.h>
#include <mycrypto/misc.hpp>

#include "utils.h"
#include "script.h"

using namespace std;

class Tx;

class TxOut {
private:
    uint64_t value;
    void* script_pubkey;
protected:
public:
    
    /**
     * @brief Initialize a TxOut instance by parsing bytes from a vector
     * of bytes.
     */
    TxOut(vector<uint8_t>& d);
    TxOut();
    uint8_t* serialize();
    uint64_t get_amount();

    ~TxOut();
};

class TxIn {
private:
    uint8_t prev_tx_id[SHA256_HASH_SIZE];
    uint32_t prev_tx_idx;
    Script script_sig;
    uint32_t sequence;    
protected:
public:
    vector<vector<uint8_t>> witenesses;
    /**
     * @brief Initialize a TxIn instance from a vector of bytes
     */
    TxIn(vector<uint8_t>& d);
    TxIn();
    /**
     * @brief get the ID of the previous transaction
     * @returns the ID of the previous transaction. As specified in Bitcoin's protocol, the ID is a SHA256_HASH
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
     * @returns the amount in Satoshi or 0 in case of error
     */
    uint64_t get_value();
    Script get_script_sig();
    ~TxIn();
};

class Tx {
private:
    // https://en.bitcoin.it/wiki/Protocol_documentation#tx
    uint32_t version = 0;
    bool witness_flag = false;
    size_t tx_in_count = 0;
    size_t tx_out_count = 0;
    vector<TxIn> tx_ins;
    vector<TxOut> tx_outs;
    // Designed by Satoshi as a way to achieve "high-frequency" transaction
    // but is later proved to be insecure, not in use.
    uint32_t locktime = 0;
    bool is_testnet = false;
protected:
public:
    /**
     * @brief Initialize an empty Transaction object. The object should usually
     * be filled by calling Tx::parse() or optionally by copy from an existing
     * Tx object.
    */
    Tx();
    /**
     * @brief Fill in the Tx instance by parsing bytes from a vector of bytes.
     * Bytes read from the vector will be removed from it.
     */
    Tx(vector<uint8_t>& d);
    /**
     * @brief Fetch transaction data (essentially a series of bytes) from a remote URL and deliver them to a vector.
     * 
     * @param tx_id The transaction ID
     * @param testnet testnet or mainnet
     * @param d a vector reference which will be resize()ed and where the bytes will be written to.
     * Given the design of the REST API, a '\0' will always be appended to make d.data() null-terminated
     * @return 0 means success, otherwise error code.
     */
    int static fetch_tx(const uint8_t tx_id[SHA256_HASH_SIZE], vector<uint8_t>& d);
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