#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <nlohmann/json.hpp>
#include <mycrypto/misc.hpp>

#include "mybitcoin/tx.h"
#include "mybitcoin/script.h"
#include "mybitcoin/utils.h"


using namespace std;
using json = nlohmann::json;


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage tx-test <since_block_height>\n");
        return EXIT_FAILURE;
    }
    int since_block_height = atoi(argv[1]);

    string post_data = R"(
        {"jsonrpc": "1.0", "method": "getbestblockhash", "params": []}
    )";
    curl_global_init(CURL_GLOBAL_ALL);
    json data = bitcoind_rpc(post_data);
    string latest_block_hash = data["result"];
    post_data = R"({
        "jsonrpc": "1.0",
        "method": "getblock",
        "params": [")" + latest_block_hash + R"("]
    })";
    
    data = bitcoind_rpc(post_data);
    int latest_height = data["result"]["height"];

    cout << "latest_block_hash: "<< latest_block_hash << endl;
    cout << "latest_height: " << latest_height << endl;

    int block_height = since_block_height;
    while (block_height <= latest_height) {
        post_data = R"({
            "jsonrpc": "1.0", "method":
            "getblockhash",
            "params": [)" + to_string(block_height) +
        "]}";    
        data = bitcoind_rpc(post_data);
        string block_hash = data["result"];
        post_data = R"({
            "jsonrpc": "1.0",
            "method": "getblock",
            "params": [")" + block_hash + R"(", 2]
        })";
        data = bitcoind_rpc(post_data);
        cout << "Testing block " << block_height
             << " (nTx: " << data["result"]["nTx"] << ")" << endl;
        
        for (int i = 0; i < data["result"]["nTx"]; ++i) {
            json tx = data["result"]["tx"][i];
            int64_t input_bytes_len;
            unique_fptr<uint8_t[]>  input_bytes(hex_string_to_bytes(
                tx["hex"].get<std::string>().c_str(), &input_bytes_len));
            vector<uint8_t> d(input_bytes_len);
            memcpy(d.data(), input_bytes.get(), input_bytes_len);
            Tx my_tx = Tx(d);
            if (my_tx.get_version() != tx["version"]) {
                cerr << i << "-th tx:\n"
                     << "Actual version: " << my_tx.get_version() << "\n"
                     << "Expect version: " << tx["version"] << "\n";
                return EXIT_FAILURE;
            }
            if (my_tx.get_locktime() != tx["locktime"]) {
                cerr << i << "-th tx:\n"
                     << "Actual locktime: " << my_tx.get_locktime() << "\n"
                     << "Expect locktime: " << tx["locktime"] << "\n";
                return EXIT_FAILURE;
            }
            if (my_tx.get_tx_in_count() != tx["vin"].size()) {
                cerr << i << "-th tx:\n"
                     << "Actual tx_in_count: " << my_tx.get_tx_in_count()
                     << "\n"
                     << "Expect tx_in_count: " << tx["vin"].size() << "\n";
                return EXIT_FAILURE;
            }
            
            vector<TxIn> tx_ins = my_tx.get_tx_ins();
            if (my_tx.get_tx_in_count() != tx_ins.size()) {
                cerr << i << "-th tx:\n"
                     << "get_tx_in_count(): " << my_tx.get_tx_in_count()
                     << "\n"
                     << "tx_ins.size(): " << tx_ins.size() << "\n";
                return EXIT_FAILURE;
            }
            for (size_t j = 0; j < tx_ins.size(); ++j) {
                vector<uint8_t> ss_bytes = tx_ins[j].get_script_sig(
                    ).serialize();
                read_variable_int(ss_bytes);
                unique_fptr<char[]> ss_hex(bytes_to_hex_string(
                    ss_bytes.data(), ss_bytes.size(), false));
                string expected_hex;
                if (i == 0 && j == 0) { // coinbase tx
                    expected_hex = tx["vin"][j]["coinbase"].get<string>();
                } else {
                    expected_hex = tx["vin"][j]["scriptSig"]["hex"].get<string>();
                }
                if (strcmp(ss_hex.get(), expected_hex.c_str()) != 0) {
                    cerr << i << "-th tx:\n"
                         << "Actual value: " << ss_hex.get() << "\n"
                         << "Expect value: "
                         << expected_hex
                         << "\n"
                         << "tx_id: " << tx["txid"] << endl;
                    return EXIT_FAILURE;
                }
                if (tx_ins[j].get_sequence() != tx["vin"][j]["sequence"]) {
                    cerr << i << "-th tx:\n"
                         << "Actual sequence: " << tx_ins[j].get_sequence()
                         << "\n"
                         << "Expect sequence: " << tx["vin"][j]["sequence"]
                         << "\n";
                    return EXIT_FAILURE;
                }
            }
            if (my_tx.get_tx_out_count() != tx["vout"].size()) {
                cerr << i << "-th tx:\n"
                     << "Actual tx_in_count: " << my_tx.get_tx_out_count()
                     << "\n"
                     << "Expect tx_in_count: " << tx["vout"].size() << "\n";
                return EXIT_FAILURE;
            }
            vector<TxOut> tx_outs = my_tx.get_tx_outs();
            if (my_tx.get_tx_out_count() != tx_outs.size()) {
                cerr << i << "-th tx:\n"
                     << "get_tx_out_count(): " << my_tx.get_tx_out_count()
                     << "\n"
                     << "tx_outs.size(): " << tx_outs.size() << "\n";
                return EXIT_FAILURE;
            }
            for (size_t j = 0; j < tx_outs.size(); ++j) {
                if (tx_outs[j].get_value() -
                    tx["vout"][j]["value"].get<double>() * 100000000 > 1) {
                    cerr << i << "-th tx:\n"
                         << "Actual value: " << tx_outs[j].get_value() << "\n"
                         << "Expect value: "
                         << tx["vout"][j]["value"].get<double>() * 100000000
                         << "\nDiff: "
                         << tx_outs[j].get_value() - (
                            tx["vout"][j]["value"].get<double>() * 100000000)
                         << "\n"
                         << "tx_id: " << tx["txid"] << endl;
                    return EXIT_FAILURE;
                }
                vector<uint8_t> script_pk_bytes = tx_outs[j].get_script_pubkey(
                    ).serialize();
                read_variable_int(script_pk_bytes);
                unique_fptr<char[]>  script_pk_hex(bytes_to_hex_string(
                    script_pk_bytes.data(), script_pk_bytes.size(), false));
                if (strcmp(script_pk_hex.get(),
                    tx["vout"][j]["scriptPubKey"]["hex"].get<string>().c_str())
                    != 0) {
                    cerr << i << "-th tx:\n"
                         << "Actual value: " << script_pk_hex.get() << "\n"
                         << "Expect value: "
                         << tx["vout"][j]["scriptPubKey"]["hex"].get<string>()
                         << "\n"
                         << "tx_id: " << tx["txid"] << endl;
                    return EXIT_FAILURE;
                }
            }
        }
        ++block_height;
    }
    curl_global_cleanup();
    return EXIT_SUCCESS;
}
