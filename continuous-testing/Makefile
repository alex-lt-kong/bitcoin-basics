OPT = -O3 -Wall -pedantic

main:   script-test.o script.o tx.o utils.o op.o
	g++ script-test.o script.o tx.o utils.o op.o -o script-test.out -lboost_random $(OPT) -lcurl -lmycrypto

script-test.o: script-test.cpp
	g++ -c script-test.cpp -o script-test.o -I../ $(OPT)

tx.o: ../src/tx.cpp ../src/tx.h
	g++ -c ../src/tx.cpp $(OPT)

op.o: ../src/op.cpp ../src/op.h
	g++ -c ../src/op.cpp $(OPT)

script.o: ../src/script.cpp ../src/script.h
	g++ -c ../src/script.cpp $(OPT)

utils.o: ../src/utils.cpp ../src/utils.h
	g++ -c ../src/utils.cpp $(OPT)

clean:
	rm *.o
	rm *.out


