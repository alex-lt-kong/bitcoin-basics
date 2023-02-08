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
            unique_byte_ptr input_bytes(hex_string_to_bytes(
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
                if (tx_ins[j].get_sequence() != tx["vin"][j]["sequence"]) {
                    cerr << i << "-th tx:\n"
                         << "Actual sequence: " << tx_ins[j].get_sequence() << "\n"
                         << "Expect sequence: " << tx["vin"][j]["sequence"] << "\n";
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
              /*  if (tx_outs[j].get_value() != tx["vout"][j]["value"]) {
                    cerr << i << "-th tx:\n"
                         << "Actual value: " << tx_outs[j].get_amount() << "\n"
                         << "Expect value: " << tx["vout"][j]["value"] << "\n";
                    return EXIT_FAILURE;
                }*/
            }
        }
        ++block_height;
    }
    curl_global_cleanup();
    return EXIT_SUCCESS;
}
