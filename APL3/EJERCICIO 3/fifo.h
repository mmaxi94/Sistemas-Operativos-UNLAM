#ifndef FIFO_H
#define FIFO_H
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void reset();
void green();
void red();
int writeFifo(char *, char *);
int readFifo(char *, char *, int);
void mostrarError();