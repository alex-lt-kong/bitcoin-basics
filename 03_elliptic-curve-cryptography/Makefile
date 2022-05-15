main.out: main.o ecc.o utils.o sha256.o hmac-sha256.o
	g++ main.o ecc.o utils.o sha256.o hmac-sha256.o -o main.out

main.o: main.cpp
	g++ -c main.cpp

ecc.o: ../ecc.cpp ../ecc.h
	g++ -c ../ecc.cpp

sha256.o: ../sha256.c ../sha256.h
	gcc -c ../sha256.c

hmac-sha256.o: ../hmac-sha256.c ../hmac-sha256.h
	gcc -c ../hmac-sha256.c

utils.o: ../utils.cpp ../utils.h
	g++ -c ../utils.cpp

clean:
	rm *.o main.out

# General syntax:
# target: dependencies
#	  action