#include <arpa/inet.h>
#include <assert.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random.hpp>
#include <sstream>
#include "utils.h"


using namespace boost::multiprecision;
using namespace std;

int512_t get_int512_from_bytes(const uint8_t* input_bytes,
    const size_t input_len, const bool bytes_in_big_endian) {

    int512_t result = 0;
    
    assert (input_len <= 64);
    // 64 bytes * 8 = 512bit, can't use sizeof(int512_t) here, int512_t's size could be greater than 64 bytes
    assert (CHAR_BIT == 8); 

    if (bytes_in_big_endian == false) {
        for (int i = input_len - 1; i >= 0; i--) {
        result = (result << 8) + input_bytes[i];
        }
    } else {
        for (size_t i = 0; i < input_len; i++) {
        result = (result << 8) + input_bytes[i];      
        }
    }
    return result;
}

void get_bytes_from_int256(const int256_t input_int,
    const bool bytes_in_big_endian, uint8_t* output_bytes) {

    const size_t INT256_SIZE = 32;
    memcpy(output_bytes, &input_int, INT256_SIZE);
    // htonl(47) == 47 means the CPU is big endian, otherwise little endian
    if ((htonl(47) == 47) != bytes_in_big_endian) {
        // That is, the CPU's endianness is different from the desired endianness
        reverse(output_bytes, output_bytes + INT256_SIZE);      
    }
}

bool fermat_primality_test(const int512_t input, const int iterations) {  
    if (input <= 1) {
        return false;
    }

    boost::random::random_device gen;
    boost::random::uniform_int_distribution<uint512_t> ui(1,
        (uint512_t)(input - 1));
    int512_t x = -1;
    for (int i = 0; i < iterations; i++) {
        x = (int512_t)ui(gen);

        if (powm(x, input - 1, input) != 1) {
        return false;
        }
    }
    return true;
}

char* encode_bytes_to_base58_string(const uint8_t* input_bytes,
    const size_t input_len, const bool bytes_in_big_endian) {
    //cout << ceil(input_len * 1.36565823) - (input_len * 1.36565823) << endl;
    size_t output_len = ceil(input_len * 1.36565823) + 1; // +1 for null-termination.
    size_t zero_count = 0;
    while (zero_count < input_len && input_bytes[zero_count] == 0)
            ++zero_count; // This is not strictly needed in the current implementation, but let's keep it anyway...
        /*
        * How do we get the size in advance? We can consider it this way--In base58 encoding, we use 58 characters to encode
        * bytes (or bits), how many bits can be represented by one character? It is log(2)58 ≈ 5.8579 bits.
        * Is it possible for base58 character to present 6 bits? No, 2^6 = 64, that is to say, characters will be exhausted
        * before 0b11 1111 can be encoded.
        * Since 1 byte = 8 bits, there are (input_len * 8) bits to be represented. Therefore, we need:
        * (input_len * 8) / log(2)58 = (input_len * log(2)256) / log(2)58 = input_len * log(58)256 ≈ input_len * 1.36565823
        */
    int512_t num = get_int512_from_bytes(input_bytes, input_len,
        bytes_in_big_endian);
    char* buf = (char*)calloc(output_len, 1);
    int idx = output_len - 2; // buf[idx-1] should remain \0 to make the string null-terminated.
    static const char b58_table[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    while (num > 0) {
        buf[idx--] = b58_table[(uint8_t)(num % 58)];
        num /= 58;    
    }

    if (zero_count > 0) { assert (zero_count == (size_t)idx); }
    while (idx > 0) {
        buf[idx--] = b58_table[0];
    }

    /* This part is a dirty hack--the issue is that I can't predict the length of the output with pinpoint
        accuracy...so here if we calloc() too many memory blocks, we shorten the string by memcpy()ing...
        So that we are sure that we wont get a string whose first byte is null...
    */
    char* buf1;
    if (buf[0] == '\0') {
        buf1 = (char*)calloc(output_len - 1, 1);
        memcpy(buf1, buf+1, output_len - 1);
        free(buf);
        return buf1;
    } else { buf1 = buf; }
    return buf1;
}

char* encode_base58_checksum(const uint8_t* input_bytes,
    const size_t input_len) {
    // return encode_base58(b + hash256(b)[:4])
    uint8_t hash[SHA256_HASH_SIZE];
    cal_sha256_hash(input_bytes, input_len, hash);
    cal_sha256_hash(hash, SHA256_HASH_SIZE, hash);  
    vector<uint8_t> base58_input(input_len + 4);

    memcpy(base58_input.data(), input_bytes, input_len);
    memcpy(base58_input.data() + input_len, hash, 4);

    return encode_bytes_to_base58_string(base58_input.data(),
        input_len + 4, true);
}

void hash160(const uint8_t* input_bytes, const size_t input_len,
    uint8_t* hash) {
    uint8_t sha256_hash[SHA256_HASH_SIZE];
    cal_sha256_hash(input_bytes, input_len, sha256_hash);
    cal_rpiemd160_hash(sha256_hash, SHA256_HASH_SIZE, hash);
}

uint64_t read_variable_int(vector<uint8_t>& d) {
    // Per C standard, shifting by a negative value or a value greater than or equal to the number of bits of
    // the left operand is undefined. We need to cast the left operand to a bigger type of integer to make it work.
    // I believe prt[1] does not need to be casted, just casting it make the code a bit prettier... 
    uint64_t val;
    if (d.size() == 0) {
        throw invalid_argument(string(__func__) + "(): vector is empty");
    }
    if (d[0] == 0xfd) {
        if (d.size() < 3) {
            throw invalid_argument(string(__func__) +
                "(): vector does not contain enough bytes");
        }
        // the next two bytes are the number      
        val = (d[1] << 0) | (d[2] << 8);
        d.erase(d.begin(), d.begin() + 3);
    } else if (d[0] == 0xfe) {
        if (d.size() < 5) {
            throw invalid_argument(string(__func__) +
                "(): vector does not contain enough bytes");
        }
        // the next four bytes are the number 
        val = ((uint64_t)d[1] <<  0) | ((uint64_t)d[2] <<  8) | 
              ((uint64_t)d[3] << 16) | ((uint64_t)d[4] << 24);
        d.erase(d.begin(), d.begin() + 5);
    } else if (d[0] == 0xff) {
        if (d.size() < 9) {
            throw invalid_argument(string(__func__) +
                "(): vector does not contain enough bytes");
        }
        // the next eight bytes are the number        
        val = (
            ((uint64_t)d[1] << 0)  | ((uint64_t)d[2] << 8)  | 
            ((uint64_t)d[3] << 16) | ((uint64_t)d[4] << 24) |
            ((uint64_t)d[5] << 32) | ((uint64_t)d[6] << 40) | 
            ((uint64_t)d[7] << 48) | ((uint64_t)d[8] << 56)
        );
        d.erase(d.begin(), d.begin() + 9);
    } else {
        val = d[0];
        d.erase(d.begin(), d.begin() + 1);
    }
    return val;
}

uint8_t* encode_variable_int(const uint64_t num, size_t* int_len) {
    uint8_t* result = nullptr;
    if (num < 0xfd) {
        *int_len = 1 + 0;
        result = (uint8_t*)malloc(*int_len);
        memcpy(result + 0, &num, 1);    
    } else if (num < 0x10000) { // 1048576, i.e., 2^20
        *int_len = 1 + 2;
        result = (uint8_t*)malloc(*int_len);
        result[0] = 0xfd;
        memcpy(result + 1, &num, 2);
    } else if (num < 0x100000000) { // 4294967296, i.e., 2^32
        *int_len = 1 + 4;
        result = (uint8_t*)malloc(*int_len);
        result[0] = 0xfe;
        memcpy(result + 1, &num, 4);
    } else {
        *int_len = 1 + 8;
        result = (uint8_t*)malloc(*int_len);
        result[0] = 0xff;
        memcpy(result + 1, &num, 8);
    }
    return result;
}

struct CurlMemoryStruct {
    uint8_t* memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size,
  size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct CurlMemoryStruct *mem = (struct CurlMemoryStruct *)userp;

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

json bitcoind_rpc(string post_data) {
    struct CurlMemoryStruct resp_body;

    resp_body.memory = (uint8_t*)malloc(1);  /* will be grown as needed by realloc above */
    resp_body.size = 0;    /* no data at this point */
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8332");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
        curl_easy_setopt(curl, CURLOPT_USERPWD, "rpcuser:rpcpassword");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp_body);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl);
        char err_msg[PATH_MAX] = {0};
        if(res != CURLE_OK) {
            snprintf(err_msg, PATH_MAX - 1, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
            fprintf(stderr, "%s\n", err_msg);
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
        if (strlen(err_msg) > 0) {
            throw runtime_error(string(err_msg));
        }
    }
    

    string jsonStr(reinterpret_cast<char*>(resp_body.memory));
    json data = json::parse(jsonStr);
    free(resp_body.memory);
    return data;
}
