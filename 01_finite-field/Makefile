main.out: main.o ecc.o
	g++ main.o ecc.o -o main.out

main.o: main.cpp
	g++ -c main.cpp

ecc.o: ../ecc.cpp
	g++ -c ../ecc.cpp

clean:
	rm *.o main.out

# General syntax:
# target: dependencies
#	  action