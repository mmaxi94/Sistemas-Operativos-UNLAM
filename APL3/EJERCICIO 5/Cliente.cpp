/*
	####################   ENCABEZADO    ##########################
	#							                                  #
	#	Nombre de los archivos: Cliente.cpp	                  #
	#	Numero de APL : 3				                          #
	#	Numero de Ejercicio: 5		                 		      #
	#							                                  #
	#	Integrantes:								              #
	#  		Rodriguez, Cesar Daniel		39166725	              #
	#  		Garcia Velez, Kevin 		38619312	              #
	# 		Morales, Maximiliano 		38176604	              #
	#                                                             #
	###############################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

void mostrarAyuda() {
    printf("\nHELP CLIENTE:\n");
    printf("\nCliente: Cliente.exe\n");
    printf("DESCRIPCION:\n");
    printf("Se le debe especificar el servidor al que desea conectarse, por ejemplo 127.0.0.1\n");
    printf("El cliente le hace peticiones al servidor local de refugio de gatos. Las peticiones son CONSULTA, BAJA o ALTA de gatos. \n");
    printf("SINTAXIS:\n");
    printf("./Cliente.exe ip\n");
    printf("Ejemplos: \n");
    printf("./Cliente.exe 127.0.0.1 ALTA Snowball siames M CA -> da de alta el gato con los datos correspondientes (ALTA Nombre Raza Sexo(M/H) y CA si esta castrado, SC si no esta castrado).\n");
    printf("./Cliente.exe 127.0.0.1 CONSULTA Snowball -> devuelve el gato que se le paso por parametro.\n");
    printf("./Cliente.exe 127.0.0.1 CONSULTA -> devuelve todos los gatos que existen en el servidor.\n");
    printf("./Cliente.exe 127.0.0.1 BAJA Snowball -> da de baja el nombre del gato que le paso por parametro.\n");
    printf("./Cliente.exe Largo máximo del nombre: 9 caracteres.\n");
    printf("./Cliente.exe Largo máximo de raza: 9 caracteres.\n");
}

typedef struct {
    char accion[9];
    char nombre[10];
    char raza[10];
    char sexo;
    char castrado[3];
}t_registro;

#define ARG_ACCION 2
#define ARG_NOMBRE 3
#define ARG_RAZA 4
#define ARG_SEXO 5
#define ARG_CASTRADO 6
#define TAM_BUFFER 500

using namespace std;

int main(int argc, char* argv[]) {


    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "--help") == 0))
    {
        mostrarAyuda();
        exit(0);
    }

    if (argc < 3 || argc > 7) {
        printf("CLIENT Error: Número de argumentos incorrecto. %d \n", argc);
        mostrarAyuda();
        exit(1);
    }

    t_registro datos;

    char *ip;
    int fdServidor, numbytes,puerto;
    char buf[TAM_BUFFER];
    char bufSend[TAM_BUFFER];
    puerto=5558;
    ip = argv[1];

    struct hostent *he; 

    struct sockaddr_in server; 

    he=gethostbyname(ip);

    fdServidor = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&server, '0', sizeof(server)); 
    server.sin_family = AF_INET;
    server.sin_port = htons(puerto);
    server.sin_addr = *((struct  in_addr *)he->h_addr);

    connect(fdServidor, (struct sockaddr *)&server, sizeof(struct sockaddr));

    if (strcmp(argv[ARG_ACCION], "ALTA") == 0) {
        if (argc < 7) {
            mostrarAyuda();
            exit(3);
        }

        if ( !strchr(argv[ARG_NOMBRE],0) || !strchr(argv[ARG_RAZA],0) || (*(argv[ARG_SEXO]) != 'M' && *(argv[ARG_SEXO]) != 'H') || !strchr(argv[ARG_CASTRADO],0) || 
            strlen(argv[ARG_NOMBRE]) == 1 || strlen(argv[ARG_RAZA]) == 1 || (strcmp(argv[ARG_CASTRADO],"CA") != 0 && strcmp(argv[ARG_CASTRADO],"SC") != 0) ) {

            printf("INGRESO INCORRECTO\n");
            mostrarAyuda();
            exit(3);
        }

        strcpy(datos.accion, argv[ARG_ACCION]);
        strcpy(datos.nombre, argv[ARG_NOMBRE]);
        strcpy(datos.raza, argv[ARG_RAZA]);
        memcpy(&(datos.sexo), argv[ARG_SEXO], 1);
        strcpy(datos.castrado, argv[ARG_CASTRADO]);

        int largoAccion = strlen(datos.accion)+1;
        int largoNombre = strlen(datos.nombre)+1;
        int largoRaza = strlen(datos.raza)+1;
        int largoCastrado = strlen(datos.castrado)+1;

        memcpy(bufSend, datos.accion, largoAccion); 
        memcpy(bufSend+largoAccion, datos.nombre, largoNombre); 
        memcpy(bufSend+largoAccion+largoNombre, datos.raza, largoRaza); 
        *(bufSend+largoAccion+largoNombre+largoRaza)= datos.sexo;
        *(bufSend+largoAccion+largoNombre+largoRaza+1)= '\0';
        memcpy(bufSend+largoAccion+largoNombre+largoRaza+2, datos.castrado, largoCastrado); 

        int cantCaracteresAEnviar = largoAccion+largoNombre+largoRaza+2+largoCastrado; 

        send(fdServidor, bufSend, cantCaracteresAEnviar/*+1*/,0); 

        numbytes = recv(fdServidor, buf, TAM_BUFFER, 0); 
        buf[numbytes] = '\0';

        if(strcmp(buf,"Gato repetido.\n") == 0 ) {
            printf("Mensaje del servidor: %s\n", buf);
        }
    } else if (strcmp(argv[ARG_ACCION], "BAJA") == 0) {

        if (argc < 4 ) {
            mostrarAyuda();
            exit(3);
        }
        if (strchr(argv[ARG_NOMBRE],'\0') == NULL || strlen(argv[ARG_NOMBRE]) == 1) {
            mostrarAyuda();
            exit(3);
        }

        strcpy(datos.accion, argv[ARG_ACCION]);
        strcpy(datos.nombre, argv[ARG_NOMBRE]);

        int largoAccion = strlen(datos.accion)+1;
        int largoNombre = strlen(datos.nombre)+1;

        memcpy(bufSend, datos.accion, largoAccion); 
        memcpy(bufSend+largoAccion, datos.nombre, largoNombre); 
        int cantCaracteresAEnviar = largoAccion+largoNombre;

        send(fdServidor, bufSend, cantCaracteresAEnviar,0); 

        numbytes = recv(fdServidor, buf, TAM_BUFFER, 0); 
        buf[numbytes] = '\0';

        if(strcmp(buf,"No es posible dar de baja a un gato inexistente en la DB\n") == 0 ) {
            printf("Mensaje del servidor: %s\n", buf);
        }
    } else if (strcmp(argv[ARG_ACCION], "CONSULTA") == 0) {

        if ( argc == 4 && (strchr(argv[ARG_NOMBRE],'\0') == NULL || strlen(argv[ARG_NOMBRE]) == 1) ) {
            mostrarAyuda();
            exit(3);
        }

        strcpy(datos.accion, argv[ARG_ACCION]);
        if(argc == 4) {
            strcpy(datos.nombre, argv[ARG_NOMBRE]);
        } else {
            strcpy(datos.nombre, "");
        }
        
        int largoAccion = strlen(datos.accion)+1; 
        int largoNombre = strlen(datos.nombre)+1; 
        
        memcpy(bufSend, datos.accion, largoAccion); 
        memcpy(bufSend+largoAccion, datos.nombre, largoNombre); 
        int cantCaracteresAEnviar = largoAccion + largoNombre;
        send(fdServidor, bufSend, cantCaracteresAEnviar,0);

        numbytes = recv(fdServidor, buf, TAM_BUFFER, 0); 
        buf[numbytes] = '\0';
        int cantBytes = numbytes;
        char* punteroABuf = buf;

        while (cantBytes > 0){
            char* finDeCadenaNombre = strchr(punteroABuf,'\0'); 

            char* finDeCadenaRaza = strchr(finDeCadenaNombre+1,'\0');
            char* caracterSexo = finDeCadenaRaza+1;
            char* finDeCadenaCastrado = strchr(caracterSexo+1,'\0');

            int largoCadenaNombre = finDeCadenaNombre - punteroABuf + 1; 

            int largoCadenaRaza = finDeCadenaRaza - (punteroABuf+largoCadenaNombre)+ 1; 

            int largoLeido = largoCadenaNombre+largoCadenaRaza+1/*de sexo*/+3 /*de castrado*/;

            t_registro registroRecibido;
            memcpy(registroRecibido.nombre, punteroABuf, largoCadenaNombre);
            memcpy(registroRecibido.raza, punteroABuf+largoCadenaNombre, largoCadenaRaza);
            registroRecibido.sexo = *(punteroABuf+largoCadenaNombre+largoCadenaRaza);
            memcpy(registroRecibido.castrado, punteroABuf+largoCadenaNombre+largoCadenaRaza+1, 3);
            
            punteroABuf += largoLeido; 

            if (strlen(registroRecibido.nombre) != 0 && strlen(registroRecibido.raza) != 0 && strlen(registroRecibido.castrado) != 0) {
                printf("Nombre: %s | ", registroRecibido.nombre);
                printf("Raza: %s | ", registroRecibido.raza);
                printf("Sexo: %c | ", registroRecibido.sexo);
                printf("Castrado: %s\n", registroRecibido.castrado);
            } 
            cantBytes -= largoLeido;
        }

        if(strcmp(buf,"No existe el gato en la DB.\n") == 0 ) {
            printf("Mensaje del servidor: %s\n", buf);
        }
    } else {
        printf("CLIENT: Se ingresó una acción incorrecta");
        mostrarAyuda();
    }
 
    close(fdServidor);
}