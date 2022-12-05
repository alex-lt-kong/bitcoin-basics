OPT = -O3 -Wall

main.out: main.o script.o tx.o utils.o sha256.o ripemd160.o misc.o
	g++ main.o script.o tx.o utils.o sha256.o ripemd160.o misc.o -o main.out -I../cryptographic-algorithms -lboost_random $(OPT) -lcurl

main.o: main.cpp ../op.h
	g++ -c main.cpp -I../ -I../cryptographic-algorithms $(OPT)

script.o: ../script.cpp ../script.h
	g++ -c ../script.cpp $(OPT)

tx.o: ../tx.cpp ../tx.h
	g++ -c ../tx.cpp -I../cryptographic-algorithms $(OPT)

sha256.o: ../cryptographic-algorithms/src/sha256.c ../cryptographic-algorithms/src/sha256.h
	gcc -c ../cryptographic-algorithms/src/sha256.c $(OPT)

ripemd160.o: ../cryptographic-algorithms/src/ripemd160.c ../cryptographic-algorithms/src/ripemd160.h
	gcc -c ../cryptographic-algorithms/src/ripemd160.c $(OPT)

misc.o: ../cryptographic-algorithms/src/misc.c ../cryptographic-algorithms/src/misc.h
	gcc -c ../cryptographic-algorithms/src/misc.c $(OPT)

utils.o: ../utils.cpp ../utils.h
	g++ -c ../utils.cpp -I../cryptographic-algorithms $(OPT)

clean:
	rm *.o
	rm *.out