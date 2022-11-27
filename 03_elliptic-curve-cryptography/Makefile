INCS = -I../cryptographic-algorithms -I../
LIBS = -lboost_random
OPT = -O3 -Wall

main.out: main.cpp ecc.o utils.o sha256.o sha1.o hmac.o misc.o ripemd160.o
	g++ main.cpp ecc.o utils.o sha256.o sha1.o hmac.o misc.o ripemd160.o -o main.out $(INCS) $(LIBS) $(OPTS)

ecc.o: ../ecc.cpp ../ecc.h sha256.o
	g++ -c ../ecc.cpp $(INCS) $(OPTS)

hmac.o: ../cryptographic-algorithms/src/hmac.c ../cryptographic-algorithms/src/hmac.h sha256.o sha1.o
	gcc -c ../cryptographic-algorithms/src/hmac.c $(OPTS)

ripemd160.o: ../cryptographic-algorithms/src/ripemd160.c ../cryptographic-algorithms/src/ripemd160.h
	gcc -c ../cryptographic-algorithms/src/ripemd160.c $(OPTS)

sha256.o: ../cryptographic-algorithms/src/sha256.c ../cryptographic-algorithms/src/sha256.h
	gcc -c ../cryptographic-algorithms/src/sha256.c $(OPTS)

sha1.o: ../cryptographic-algorithms/src/sha1.c ../cryptographic-algorithms/src/sha1.h
	gcc -c ../cryptographic-algorithms/src/sha1.c $(OPTS)

misc.o: ../cryptographic-algorithms/src/misc.c ../cryptographic-algorithms/src/misc.h
	gcc -c ../cryptographic-algorithms/src/misc.c $(OPTS)

utils.o: ../utils.cpp ../utils.h
	g++ -c ../utils.cpp $(INCS) $(OPTS)


clean:
	rm *.o main.out

# General syntax:
# target: dependencies
#	  action