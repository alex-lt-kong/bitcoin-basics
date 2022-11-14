# bitcoin-fundamentals

* Implementation of [examples](https://github.com/jimmysong/programmingbitcoin) 
in Jimmy Song's [Programming Bitcoin](https://www.oreilly.com/library/view/programming-bitcoin/9781492031482/) and more:
  * Jimmy's implementation is in Python, which hides a lot of interesting details. This repo implements
    everything in "C-style" C++--thus we capture a lot more details then original book.
  * The book implements ECDSA only--all other cryptographic algorithms are from Python's libraries. Here,
    no cryptography libraries needed--all the necessary cryptographic algorithms are either implemented in this
    repo or in the [cryptographic-algorithms](https://github.com/alex-lt-kong/cryptographic-algorithms) 
repo

* Boost library is needed for 256-bit integer support: `apt install libboost-all-dev` 

* The book's analogy on ECDSA makes the thing even more difficult to understand, may use this instead: https://cryptobook.nakov.com/digital-signatures/ecdsa-sign-verify-messages

* The project is developed on little-endian architectures, some bitwise operations will definitely **not** work on big-endian architectures.
  Fortunately, Intel, AMD and most ARM CPUs are little-endian.

## Repo structure

* `ecc.h`/`ecc.cpp`: implementation of the ECDSA algorithm from scratch.
* `tx.h`/`tx.cpp`: transaction parser.
