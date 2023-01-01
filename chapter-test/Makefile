OPT = -O3 -Wall
INCS = -I../

main.out: test_ch01.o test_ch02.o test_ch03.o test_ch04.o test_ch05.o test_ch06.o script.o tx.o utils.o ecc.o op.o

	g++ test_ch01.o               utils.o ecc.o      -o test_ch01.out -lboost_random $(OPT)        -lcriterion -lmycrypto
	g++ test_ch02.o                                  -o test_ch02.out                $(OPT)
	g++ test_ch03.o               utils.o ecc.o      -o test_ch03.out -lboost_random $(OPT)        -lcriterion -lmycrypto
	g++ test_ch04.o               utils.o ecc.o      -o test_ch04.out -lboost_random $(OPT) -lcurl -lcriterion -lmycrypto
	g++ test_ch05.o script.o tx.o utils.o       op.o -o test_ch05.out -lboost_random $(OPT) -lcurl -lmycrypto
	g++ test_ch06.o script.o tx.o utils.o       op.o -o test_ch06.out -lboost_random $(OPT) -lcurl -lmycrypto
	

test_ch01.o: test_ch01.cpp
	g++ -c test_ch01.cpp $(INCS) $(OPT)
test_ch02.o: test_ch02.cpp
	g++ -c test_ch02.cpp $(INCS) $(OPT)
test_ch03.o: test_ch03.cpp
	g++ -c test_ch03.cpp $(INCS) $(OPT)
test_ch04.o: test_ch04.cpp
	g++ -c test_ch04.cpp $(INCS) $(OPT)
test_ch05.o: test_ch05.cpp
	g++ -c test_ch05.cpp $(INCS) $(OPT)
test_ch06.o: test_ch06.cpp ../src/op.h
	g++ -c test_ch06.cpp $(INCS) $(OPT)
utils.o: ../src/utils.cpp ../src/utils.h
	g++ -c ../src/utils.cpp $(OPT)

script.o: ../src/script.cpp ../src/script.h
	g++ -c ../src/script.cpp $(OPT)
tx.o: ../src/tx.cpp ../src/tx.h
	g++ -c ../src/tx.cpp $(OPT)
op.o: ../src/op.cpp ../src/op.h
	g++ -c ../src/op.cpp $(OPT)
ecc.o: ../src/ecc.cpp ../src/ecc.h
	g++ -c ../src/ecc.cpp $(OPT)

clean:
	rm *.o
	rm *.out