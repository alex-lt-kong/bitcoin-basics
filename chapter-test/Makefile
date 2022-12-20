OPT = -O3 -Wall
INCS = -I../cryptographic-algorithms -I../

main.out: test_ch01.o test_ch02.o test_ch03.o test_ch04.o test_ch05.o test_ch06.o script.o tx.o utils.o sha256.o ripemd160.o misc.o ecc.o hmac.o sha1.o op.o

	g++ test_ch01.o               utils.o ecc.o hmac.o      -o test_ch01.out                               -lboost_random $(OPT)        -lcriterion -lmycrypto
	g++ test_ch02.o                                         -o test_ch02.out                                              $(OPT)
	g++ test_ch03.o               utils.o ecc.o hmac.o      -o test_ch03.out                               -lboost_random $(OPT)        -lcriterion -lmycrypto
	g++ test_ch04.o               utils.o ecc.o hmac.o      -o test_ch04.out -I../cryptographic-algorithms -lboost_random $(OPT) -lcurl -lcriterion -lmycrypto
	g++ test_ch05.o script.o tx.o utils.o              op.o -o test_ch05.out -I../cryptographic-algorithms -lboost_random $(OPT) -lcurl -lcriterion -lmycrypto
	g++ test_ch06.o script.o tx.o utils.o              op.o -o test_ch06.out -I../cryptographic-algorithms -lboost_random $(OPT) -lcurl -lcriterion -lmycrypto
	

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
	g++ -c ../src/utils.cpp -I../cryptographic-algorithms $(OPT)

script.o: ../src/script.cpp ../src/script.h
	g++ -c ../src/script.cpp $(OPT)
tx.o: ../src/tx.cpp ../src/tx.h
	g++ -c ../src/tx.cpp -I../cryptographic-algorithms $(OPT)
op.o: ../src/op.cpp ../src/op.h
	g++ -c ../src/op.cpp -I../cryptographic-algorithms $(OPT)
ecc.o: ../src/ecc.cpp ../src/ecc.h
	g++ -c ../src/ecc.cpp -I../cryptographic-algorithms $(OPT)

clean:
	rm *.o
	rm *.out