* This chapter explain quite a lot of the concept "finite field". Mathematically,
  a finite field is a field that contains a finite number of elements. As with
  any field, a finite field is a set on which the operations of multiplication,
  addition, subtraction and division are defined and satisfy certain basic rules.
  The most common examples of finite fields are given by the integers mod *p* when
  *p* is a prime number.

* One full C++ class is dedicated to implement the concept of "finite field" in this
  chapter.
  
* However, one should be aware that, in a lot of programming languages,
  a wide variety of `unsigned int`s are finite fields by default.

* For example, a typical 32-bit `unsigned int` is a finite field element whose
  order is `2^32`

* Can `unsigned int` overflow? In C99 standard, it explicitly defines
  that "a computation involving unsigned operands can **never overﬂow**,
  because a result that cannot be represented by the resulting unsigned integer
  type is reduced modulo the number that is one greater than the largest
  value that can be represented by the resulting type."

* Note that while `unsigned int`s can never overflow, `signed int`s can overflow
  and the behavior in C/C++ is explicitly undefined.

## Sample output
```
testAddition():
Result: 1
Expect: 1


testMultiplication():
Result: 1
Expect: 1
Result: 10 (13)
Expect: 10 (13)
Result: 23 (97)
Expect: 23 (97)
Result: 68 (97)
Expect: 68 (97)

testPower():
Result: 1
Expect: 1
Result: 63 (97)Expect: 63 (97)

Test division:
Result: 3 (19)
Expect: 3 (19)
Result: 9 (19)
Expect: 9 (19)

exercise8():
Result: 4 (31)
Expect: 4 (31)
Result: 29 (31)
Expect: 29 (31)
Result: 13 (31)
Expect: 13 (31)

test_fermat_primality_test():
2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 
```