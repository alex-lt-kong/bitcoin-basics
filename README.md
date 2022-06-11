# bitcoin-fundamentals

* C/C++ implementation of Python [examples](https://github.com/jimmysong/programmingbitcoin) 
in Jimmy Song's [Programming Bitcoin](https://www.oreilly.com/library/view/programming-bitcoin/9781492031482/).
(For learning purpose, many components, especially those involving bit/byte-level manipulation, are in "C-style" C++, rather than
"modern-style" C++.)

* Boost library is needed for 256-bit integer support: `apt install libboost-all-dev` 

* No third-party cryptography libraries needed--all the necessary cryptographic algorithms are either
implemented in this repo or in the [cryptographic-algorithms](https://github.com/alex-lt-kong/cryptographic-algorithms) 
repo.

* The book's analogy on ECDSA makes the thing even more difficult to understand, may use this instead: https://cryptobook.nakov.com/digital-signatures/ecdsa-sign-verify-messages