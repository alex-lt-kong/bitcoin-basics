OPT = -O3 -Wall

main.out: main.o ecc.o utils.o sha256.o sha1.o hmac.o misc.o ripemd160.o
	g++ main.o ecc.o utils.o sha256.o sha1.o hmac.o misc.o ripemd160.o -o main.out -lboost_random $(OPT)

main.o: main.cpp
	g++ -c main.cpp -I../ -I../cryptographic-algorithms $(OPT)

ecc.o: ../ecc.cpp ../ecc.h
	g++ -c ../ecc.cpp -I../cryptographic-algorithms $(OPT)

hmac.o: ../cryptographic-algorithms/src/hmac.c ../cryptographic-algorithms/src/hmac.h
	gcc -c ../cryptographic-algorithms/src/hmac.c $(OPT)

sha256.o: ../cryptographic-algorithms/src/sha256.c ../cryptographic-algorithms/src/sha256.h
	gcc -c ../cryptographic-algorithms/src/sha256.c $(OPT)

ripemd160.o: ../cryptographic-algorithms/src/ripemd160.c ../cryptographic-algorithms/src/ripemd160.h
	gcc -c ../cryptographic-algorithms/src/ripemd160.c $(OPT)

sha1.o: ../cryptographic-algorithms/src/sha1.c ../cryptographic-algorithms/src/sha1.h
	gcc -c ../cryptographic-algorithms/src/sha1.c $(OPT)

misc.o: ../cryptographic-algorithms/src/misc.c ../cryptographic-algorithms/src/misc.h
	gcc -c ../cryptographic-algorithms/src/misc.c $(OPT)

utils.o: ../utils.cpp ../utils.h
	g++ -c ../utils.cpp -I../cryptographic-algorithms $(OPT)

clean:
	rm *.o main.out

# General syntax:
# target: dependencies
#	  action