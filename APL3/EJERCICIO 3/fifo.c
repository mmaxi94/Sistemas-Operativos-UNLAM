#include "fifo.h"

/*
    ####################   ENCABEZADO    ##########################
    #							                                                   #
    #	Nombre de los archivos: cliente.c servidor.c fifo.c fifo.h           #
    #	Numero de APL : 3				                                          #
    #	Numero de Ejercicio: 3		                 		                     #
    #							                                                   #
   #	Integrantes:								                                 #
    #    Rodriguez, Cesar Daniel		39166725	                              #
    # 	Bastante, Javier 			   38621360	                              #
    #  	Garcia Velez, Kevin 		   38619312	                              #
    # 	Morales, Maximiliano 		38176604	                              #
    #		Baranda, Leonardo 			36875068	                              #
    #                                                                      #
    ###############################################################
*/

int writeFifo(char * fifo, char *  mensaje) {
   int t = open(fifo, O_WRONLY);
   write(t, mensaje, strlen(mensaje));
   close(t);
}

int readFifo(char * fifo, char * mensaje, int tam) {
   int t = open(fifo, O_RDONLY);
   read(t, mensaje, tam);
   close(t);
}

void green(){
	printf("\033[1;32m");
}

void reset(){
	printf("\033[0m");
}

void red(){
	printf("\033[1;31m");
}

void mostrarError() {
   red();
   puts("Error en la ejecucion del proceso, para más informacion, ejecutar el proceso con los parametros -h o --help");
   reset();
}