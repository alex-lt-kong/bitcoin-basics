# libmybitcoin

* The repo is still a work in progress..

* The repo starts as implementation of [examples](https://github.com/jimmysong/programmingbitcoin) 
in Jimmy Song's [Programming Bitcoin](https://www.oreilly.com/library/view/programming-bitcoin/9781492031482/)
and it evolves to be a full-fledged bitcoin library.
* It differs from Jimmy's book in many ways, including:

    1. The book's implementation is in Python, which hides a lot of interesting
    (or gory if you prefer) details. This repo implements everything in C++--
    thus we can get down to the nitty-gritty of Bitcoin.
    1. The book's implementation includes minimal test cases, which only
    aim to demonstrate the basic idea of Bitcoin's design. These tests fail to
    test the program against a wide variety of "non-standard" transactions
    on the Bitcoin's blockchain. Actually many valid transactions on the
    Bitcoin's mainnet can break Jimmy's implementation. This repo attempts to
    test the implementation against each and single transaction on the Bitcoin
    mainnet blockchain, making sure that it is fully compatible with Bitcoin
    in as we know in real life.
    1. The book's version of "Bitcoin" is not up-to-date and misses a few
    patches to it, such as [BIP141](https://en.wikipedia.org/wiki/SegWit). They
    are fully implemented in this repo.
    1. The book implements ECDSA only--all other cryptographic algorithms
    are from Python libraries. This repo does not use any third-party
    cryptography libraries. All the necessary cryptographic algorithms are
    either implemented right in this repo or in the
    [libmycrypto](https://github.com/alex-lt-kong/libmycrypto) repo.

* Other points to note:
  * The book's analogy on ECDSA makes the thing even more difficult to
  understand, may use [this](https://cryptobook.nakov.com/digital-signatures/ecdsa-sign-verify-messages)
  instead.
  * The project is developed on little-endian architectures, some bitwise
  operations will definitely **not** work on big-endian architectures.
  Fortunately, Intel, AMD and most ARM CPUs are little-endian.

## Dependencies

* Boost for 256-bit integer support: `apt install libboost-all-dev` 
* cURL for network communication: `apt install libcurl4-gnutls-dev`
* [libmycrypto](https://github.com/alex-lt-kong/libmycrypto) for cryptographic
algorithms.
* `nlohmann-json` for JSON support: `apt install nlohmann-json3-dev`
* [Bitcoin Core](https://github.com/bitcoin/bitcoin), the official Bitcoin
client/library: used to query previous transactions and perform continuous
testing.
  * Its [`bitcoin.conf`](https://manpages.org/bitcoinconf/5) should at least
  contain:
  ```
  rpcpassword=rpcpassword
  rpcuser=rpcuser
  txindex=1
  ```
  * As `bitcoind` is exposed to localhost by default, `rpcuser` and
  `rpcpassword` are hard coded in this repo.
  * We need to invoke `bitcoin-cli getrawtransaction <tx_id>`, so we want
  to enable transaction indexing.

### Dev dependencies

* [blockstream](https://blockstream.info/api/)'s RESTful API services: used to
perform continuous testing.
* `confluent_kafka` for Kafka client: `pip3 install confluent_kafka`
* `pycryptodome` for Kafka encryption: `pip3 install pycryptodome`
* `crypto++` for encrypting data over Kafka: `apt install libcrypto++-dev`

## Repo structure

* `chapter-test`: driver functions that run unit tests on Bitcoin client's
functionalities. There is one test driver source code file
for each chapter in Jimmy's book
* `src`: source code that implements a Bitcoin client
  * `ecc.h`/`ecc.cpp`: implementation of the ECDSA algorithm from scratch.
  * `script.cpp`/`script.h`: parser and serializer of Bitcoin's Script language.
  * `tx.h`/`tx.cpp`: transaction parser and serializer.
  * `op.h`/`op.cpp`: define operations of Bitcoin's Script virtual machine.
  * `utils.h`/`utils.cpp`: utility functions
* `continuous-testing`: driver functions that test some critical functionalities
against the result from a Bitcoin blockchain explorer called
[Blockstream explorer](https://blockstream.info).
  * `script-test-driver.py`: Parse all Script components of Bitcoin blocks
  against Blockstream exlporer. Apache Kafka is supported to send real-time
  test results to any interested clients.
  * `tx-text.cpp`: Parse transactions from Bitcoin blocks against Bitcoin Core's
  `bitcoind` daemon.

## Quality assurance


* Instead of `cmake ../`, run `cmake .. -DBUILD_ASAN=ON` then `make test` to
test memory error with
[AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer).
* The repo is also tested with `Valgrind` from time to time:
`valgrind --leak-check=yes --log-file=valgrind.rpt ./build/chapter-test/test_ch01`.
Unfortunately, this part is not automated.


## `bitcoin-cli` useful commands and their Python equivalents

* Get block hash by block height: `bitcoin-cli getblockhash 123456`
* Get block content by block hash: `bitcoin-cli getblock 0000000000002917ed80650c6174aac8dfc46f5fe36480aaef682ff6cd83c3ca [0,1,2]`
* Get transaction by transaction id: `bitcoin-cli getrawtransaction da917699942e4a96272401b534381a75512eeebe8403084500bd637bd47168b3`
* Get information of nodes currently being connected to: `bitcoin-cli getpeerinfo`


### Python one-liner

```Python
import requests
result = requests.post(url='http://127.0.0.1:8332', auth=('rpcuser', 'rpcpassword'), json={"method": "getblock", "params": ['0000000000002917ed80650c6174aac8dfc46f5fe36480aaef682ff6cd83c3ca', 1]}).json()['result']
```