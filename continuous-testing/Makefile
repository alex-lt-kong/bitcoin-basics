OPT = -O3 -Wall

main:   script-test.o script.o tx.o utils.o sha256.o ripemd160.o misc.o op.o
	g++ script-test.o script.o tx.o utils.o sha256.o ripemd160.o misc.o op.o -o script-test.out -I../cryptographic-algorithms -lboost_random $(OPT) -lcurl

script-test.o: script-test.cpp
	g++ -c script-test.cpp -o script-test.o -I../ -I../cryptographic-algorithms $(OPT)

tx.o: ../src/tx.cpp ../src/tx.h
	g++ -c ../src/tx.cpp -I../cryptographic-algorithms $(OPT)

op.o: ../src/op.cpp ../src/op.h
	g++ -c ../src/op.cpp -I../cryptographic-algorithms $(OPT)

script.o: ../src/script.cpp ../src/script.h
	g++ -c ../src/script.cpp -I../cryptographic-algorithms $(OPT)

sha256.o: ../cryptographic-algorithms/src/sha256.c ../cryptographic-algorithms/src/sha256.h
	gcc -c ../cryptographic-algorithms/src/sha256.c $(OPT)

ripemd160.o: ../cryptographic-algorithms/src/ripemd160.c ../cryptographic-algorithms/src/ripemd160.h
	gcc -c ../cryptographic-algorithms/src/ripemd160.c $(OPT)

misc.o: ../cryptographic-algorithms/src/misc.c ../cryptographic-algorithms/src/misc.h
	gcc -c ../cryptographic-algorithms/src/misc.c $(OPT)

utils.o: ../src/utils.cpp ../src/utils.h
	g++ -c ../src/utils.cpp -I../cryptographic-algorithms $(OPT)

clean:
	rm *.o
	rm *.out