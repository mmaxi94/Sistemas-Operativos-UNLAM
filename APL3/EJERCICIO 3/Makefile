
all: client server

server: servidor.o fifo.o
	gcc -o server servidor.o fifo.o

lectorFifo.o: servidor.c fifo.h

client: cliente.o fifo.o
	gcc -o client cliente.o fifo.o

cliente.o: cliente.c fifo.h

fifo.o: fifo.c

clean: 
	rm -f *.o