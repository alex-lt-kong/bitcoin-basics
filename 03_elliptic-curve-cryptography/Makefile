main.out: main.o ecc.o utils.o
	g++ main.o ecc.o utils.o -lcryptopp -o main.out

main.o: main.cpp
	g++ -c main.cpp

ecc.o: ../ecc.cpp ../ecc.h
	g++ -c ../ecc.cpp

utils.o: ../utils.cpp
	g++ -c ../utils.cpp -lcryptopp

clean:
	rm *.o main.out

# General syntax:
# target: dependencies
#	  action