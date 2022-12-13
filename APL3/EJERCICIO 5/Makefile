all: servidor cliente

servidor:
	g++ Servidor.cpp -o Servidor.exe -l pthread

cliente:
	g++ Cliente.cpp -o Cliente.exe

clean:
	rm -fv Cliente.exe Servidor.exe
