main.out: main.o tx.o utils.o sha256.o ripemd160.o misc.o
	g++ main.o tx.o utils.o sha256.o ripemd160.o misc.o -o main.out -I../cryptographic-algorithms -lboost_random

main.o: main.cpp
	g++ -c main.cpp -I../ -I../cryptographic-algorithms

tx.o: ../tx.cpp ../tx.h
	g++ -c ../tx.cpp -I../cryptographic-algorithms

sha256.o: ../cryptographic-algorithms/sha256.c ../cryptographic-algorithms/sha256.h
	gcc -c ../cryptographic-algorithms/sha256.c

ripemd160.o: ../cryptographic-algorithms/ripemd160.c ../cryptographic-algorithms/ripemd160.h
	gcc -c ../cryptographic-algorithms/ripemd160.c

misc.o: ../cryptographic-algorithms/misc.c ../cryptographic-algorithms/misc.h
	gcc -c ../cryptographic-algorithms/misc.c

utils.o: ../utils.cpp ../utils.h
	g++ -c ../utils.cpp -I../cryptographic-algorithms

clear:
	rm *.o
	rm *.out