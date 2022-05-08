main.out: main.o field-element-point.o field-element.o
	g++ main.o field-element-point.o field-element.o -o main.out

main.o: main.cpp
	g++ -c main.cpp

field-element-point.o: field-element-point.cpp field-element-point.h field-element.o
	g++ -c field-element-point.cpp

field-element.o: ../01_finite-field/field-element.cpp ../01_finite-field/field-element.h
	g++ -c ../01_finite-field/field-element.cpp

clean:
	rm *.o main.out
# General syntax:
# target: dependencies
#	  action