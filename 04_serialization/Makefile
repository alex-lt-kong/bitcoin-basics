main.out: main.o ecc.o utils.o sha256.o sha1.o hmac.o misc.o ripemd160.o
	g++ main.o ecc.o utils.o sha256.o sha1.o hmac.o misc.o ripemd160.o -o main.out -lboost_random

main.o: main.cpp
	g++ -c main.cpp -I../ -I../cryptographic-algorithms

ecc.o: ../ecc.cpp ../ecc.h
	g++ -c ../ecc.cpp -I../cryptographic-algorithms

hmac.o: ../cryptographic-algorithms/hmac.c ../cryptographic-algorithms/hmac.h
	gcc -c ../cryptographic-algorithms/hmac.c

sha256.o: ../cryptographic-algorithms/sha256.c ../cryptographic-algorithms/sha256.h
	gcc -c ../cryptographic-algorithms/sha256.c

ripemd160.o: ../cryptographic-algorithms/ripemd160.c ../cryptographic-algorithms/ripemd160.h
	gcc -c ../cryptographic-algorithms/ripemd160.c

sha1.o: ../cryptographic-algorithms/sha1.c ../cryptographic-algorithms/sha1.h
	gcc -c ../cryptographic-algorithms/sha1.c

misc.o: ../cryptographic-algorithms/misc.c ../cryptographic-algorithms/misc.h
	gcc -c ../cryptographic-algorithms/misc.c

utils.o: ../utils.cpp ../utils.h
	g++ -c ../utils.cpp -I../cryptographic-algorithms

clean:
	rm *.o main.out

# General syntax:
# target: dependencies
#	  action