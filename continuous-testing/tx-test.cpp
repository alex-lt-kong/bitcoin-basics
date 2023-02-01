#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

#include <jsoncpp/json/json.h>

#include <mycrypto/misc.h>

#include "mybitcoin/tx.h"
#include "mybitcoin/script.h"
#include "mybitcoin/utils.h"
#include "script-test.h"


using namespace std;

struct MemoryStruct {
    uint8_t* memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    uint8_t *ptr = (uint8_t*)realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main(int argc, char **argv) {
    if (argc != 3) {
     //   fprintf(stderr, "Usage tx-test.out <tx_hex> <op>\n");
       // return 1;
    }

    CURL *curl;
    CURLcode res;
    struct MemoryStruct resp_body;
    struct MemoryStruct resp_header;

    resp_body.memory = (uint8_t*)malloc(1);  /* will be grown as needed by realloc above */
    resp_body.size = 0;    /* no data at this point */
    resp_header.memory = (uint8_t*)malloc(1);  /* will be grown as needed by realloc above */
    resp_header.size = 0;    /* no data at this point */
    char *post_this = "{\"jsonrpc\": \"1.0\", \"method\": \"getblock\", \"params\": [\"000000007bc154e0fa7ea32218a72fe2c1bb9f86cf8c9ebf9a715ed27fdb229a\"]}";
    // https://docs.rundeck.com/docs/api/rundeck-api.html#password-authentication
    // should be something like "j_username=admin&j_password=admin"
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
        just as well be an https:// URL if that is what should receive the
        data. */
    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8332");
    // $RUNDECK_SERVER_URL/j_security_check e.g.,: http://localhost:4440/j_security_check
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_this);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_USERPWD, "rpcuser:rpcpassword");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp_body);
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, (void *)&resp_header);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    struct curl_slist *list = NULL;
    list = curl_slist_append(list, "content-type: text/plain;");
    // libcurl will add this for us implicitly if we don't do it ourselves.
    // this hides a bit of HTTP protocol's details--say we want to POST a JSON object, we need to set it to
    // "Content-Type: application/json" instead
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    // This will make libcurl print a lot of information, but CURLOPT_POSTFIELDS won't be among it.
    // To examine the complete POST request in bytes, it seems to be easier to use another tool such as Wireshark.
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl);
    curl_slist_free_all(list);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("=====resp_body.memory=====\n[%s]\n=====resp_body.memory=====\n", resp_body.memory);
            printf("\n");
            printf("=====resp_header.memory=====\n%s\n=====resp_header.memory=====\n", resp_header.memory);
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();


    JSONCPP_STRING err;
    Json::Value root;


    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(reinterpret_cast<char*>(resp_body.memory), reinterpret_cast<char*>(resp_body.memory) + resp_body.size, &root,
                        &err)) {
        std::cout << "error" << std::endl;
        return EXIT_FAILURE;
    }
    const std::string name = root["result"]["hash"].asString();
    const int age = root["result"]["height"].asInt();

    std::cout << name << std::endl;
    std::cout << age << std::endl;
    return EXIT_SUCCESS;

/*

    int64_t input_bytes_len;
    uint8_t* input_bytes = hex_string_to_bytes(argv[1], &input_bytes_len);
    if (input_bytes_len < 0) {
        fprintf(stderr, "hex_string_to_bytes() failed\n");
        return EXIT_FAILURE;
    }
    vector<uint8_t> d(input_bytes_len);
    memcpy(d.data(), input_bytes, input_bytes_len);
    Tx my_tx = Tx();
    bool retval = my_tx.parse(d);
    if (retval == false) {
        fprintf(stderr, "Tx.parse(d) failed");
        return EXIT_FAILURE;
    }
    if (strcmp(argv[2], "get_fee") == 0) {
        printf("%u\n", my_tx.get_fee());        
    } else if (strcmp(argv[2], "get_tx_in_count") == 0) {
        printf("%u\n", my_tx.get_tx_in_count());        
    } else if (strcmp(argv[2], "get_tx_out_count") == 0) {
        printf("%u\n", my_tx.get_tx_out_count());        
    } else {
        fprintf(stderr, "unknown op\n");
    }
    return EXIT_SUCCESS;*/
}
