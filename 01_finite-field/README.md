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