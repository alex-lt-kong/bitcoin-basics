INCS = -I../cryptographic-algorithms -I../ -lboost_random

main.out: main.cpp ecc.o utils.o sha256.o sha1.o hmac.o misc.o ripemd160.o
	g++ main.cpp ecc.o utils.o sha256.o sha1.o hmac.o misc.o ripemd160.o -o main.out $(INCS)

ecc.o: ../ecc.cpp ../ecc.h sha256.o
	g++ -c ../ecc.cpp $(INCS)

hmac.o: ../cryptographic-algorithms/hmac.c ../cryptographic-algorithms/hmac.h sha256.o sha1.o
	gcc -c ../cryptographic-algorithms/hmac.c

ripemd160.o: ../cryptographic-algorithms/ripemd160.c ../cryptographic-algorithms/ripemd160.h
	gcc -c ../cryptographic-algorithms/ripemd160.c

sha256.o: ../cryptographic-algorithms/sha256.c ../cryptographic-algorithms/sha256.h
	gcc -c ../cryptographic-algorithms/sha256.c

sha1.o: ../cryptographic-algorithms/sha1.c ../cryptographic-algorithms/sha1.h
	gcc -c ../cryptographic-algorithms/sha1.c

misc.o: ../cryptographic-algorithms/misc.c ../cryptographic-algorithms/misc.h
	gcc -c ../cryptographic-algorithms/misc.c

utils.o: ../utils.cpp ../utils.h
	g++ -c ../utils.cpp $(INCS)


clean:
	rm *.o main.out

# General syntax:
# target: dependencies
#	  action