all: servidor cliente
	
servidor:
	g++ servidor4.cpp -o servidor.exe -l pthread -l rt

cliente:
	g++ cliente4.cpp -o cliente.exe -lrt -l pthread

clean:
	rm -f servidor.exe && rm -f cliente.exe