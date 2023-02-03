#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <nlohmann/json.hpp>

#include <mycrypto/misc.hpp>

#include "mybitcoin/tx.h"
#include "mybitcoin/script.h"
#include "mybitcoin/utils.h"
#include "script-test.h"


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
            Tx my_tx = Tx();
            bool retval = my_tx.parse(d);
            if (retval == false) {
                fprintf(stderr, "Tx.parse(d) failed");
                return EXIT_FAILURE;
            }
            if (my_tx.get_version() != tx["version"]) {
                fprintf(stderr, 
                "%d-th tx:\nActual version: %u\nExpect version: %u",
                i, my_tx.get_tx_in_count(), tx["version"].get<uint32_t>());
                return EXIT_FAILURE;
            }
            if (my_tx.get_tx_in_count() != tx["vin"].size()) {
                fprintf(stderr, 
                "%d-th tx:\nActual tx_in_count: %u\nExpect tx_in_count: %lu",
                i, my_tx.get_tx_in_count(), tx["vin"].size());
                return EXIT_FAILURE;
            }
            if (my_tx.get_tx_out_count() != tx["vout"].size()) {
                fprintf(stderr, 
                "%d-th tx:\nActual tx_out_count: %u\nExpect tx_out_count: %lu",
                i, my_tx.get_tx_out_count(), tx["vout"].size());
                return EXIT_FAILURE;
            }
        }
        ++block_height;
    }
    curl_global_cleanup();
    return EXIT_SUCCESS;
}
