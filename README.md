# bitcoin-internals

* The repo is still a work in progress..

* Implementation of [examples](https://github.com/jimmysong/programmingbitcoin) 
in Jimmy Song's [Programming Bitcoin](https://www.oreilly.com/library/view/programming-bitcoin/9781492031482/) and more:
  * The book's implementation is in Python, which hides a lot of interesting details. This repo implements
    everything in C++--thus we can get down to the nitty-gritty of Bitcoin.
  * The book's implementation includes minimal test cases, which only demonstrates the basic idea of Bitcoin's design,
  but fails to test the program against a wide variety of "non-standard" transactions on the blockchain. Actually
  many valid transactions on the Bitcoin's mainnet can break Jimmy's implementation. This repo aims to test its
  implementation again each and single transaction on the mainnet blockchain, making sure that it is fully compatible
  with Bitcoin in practice.
  * The book implements ECDSA only--all other cryptographic algorithms are from Python libraries. This repo does not
  use any third-party cryptography libraries--all the necessary cryptographic algorithms are either implemented right
  in this repo or in the [libmycrypto](https://github.com/alex-lt-kong/libmycrypto) repo.

* Other points to note:
  * The book's analogy on ECDSA makes the thing even more difficult to understand, may use
  [this](https://cryptobook.nakov.com/digital-signatures/ecdsa-sign-verify-messages) instead.
  * The project is developed on little-endian architectures, some bitwise operations will definitely **not** work on big-endian architectures.
  Fortunately, Intel, AMD and most ARM CPUs are little-endian.

## Dependancies:

* Boost for 256-bit integer support: `apt install libboost-all-dev` 
* cURL for network communication: `apt-get install libcurl4-gnutls-dev`
* [libmycrypto](https://github.com/alex-lt-kong/libmycrypto) for cryptographic algorithms.

## Repo structure

* `chapter-test`: driver functions that run unit tests on Bitcoin client's functionalities. There is one test driver source code file
for each chapter in Jimmy's book
* `src`: source code that implements a Bitcoin client
  * `ecc.h`/`ecc.cpp`: implementation of the ECDSA algorithm from scratch.
  * `script.cpp`/`script.h`: parser and serializer of Bitcoin's Script language.
  * `tx.h`/`tx.cpp`: transaction parser and serializer.
  * `op.h`/`op.cpp`: define operations of Bitcoin's Script virtual machine.
  * `utils.h`/`utils.cpp`: utility functions
* `continuous-testing`: driver functions that test some critical functionalities
against the result from a Bitcoin blockchain explorer called
[Blockstream explorer](https://blockstream.info)'s.
  * `script-test-driver.py`: Parse all Script components of Bitcoin blocks
  against Blockstream exlporer.

