all: Ejercicio2 clean
Ejercicio2: Ejercicio2.o
	g++ -pthread -o Ejercicio2 Ejercicio2.o

Ejercicio2.o: Ejercicio2.cpp
	g++ -c Ejercicio2.cpp

clean: 
	rm *.o