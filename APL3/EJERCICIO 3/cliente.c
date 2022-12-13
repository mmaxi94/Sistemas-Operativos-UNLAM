/*
    ####################   ENCABEZADO    ##########################
    #							                                                   #
    #	Nombre de los archivos: cliente.c servidor.c fifo.c fifo.h           #
    #	Numero de APL : 3				                                          #
    #	Numero de Ejercicio: 3		                 		                     #
    #							                                                   #
   #	Integrantes:								                                 #
    #    Rodriguez, Cesar Daniel		39166725	                              #
    #  	Garcia Velez, Kevin 		   38619312	                              #
    # 	Morales, Maximiliano 		38176604	                              #
    #                                                                      #
    ###############################################################
*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "fifo.h"

#define STOCK 1
#define SIN_STOCK 2
#define REPO 3
#define LIST 4
#define QUIT 5

/* Filename for only_one_instance() lock. */
#define INSTANCE_LOCK "rosetta-code-lock"

void fail(const char *message) {
	perror(message);
	exit(1);
}

/* Path to only_one_instance() lock. */
static char *ooi_path;

void ooi_unlink(void) {
	unlink(ooi_path);
}

/* Exit if another instance of this program is running. */
void only_one_instance(void) {
	struct flock fl;
	size_t dirlen;
	int fd;
	char *dir;

	/*
	 * Place the lock in the home directory of this user;
	 * therefore we only check for other instances by the same
	 * user (and the user can trick us by changing HOME).
	 */
	dir = getenv("HOME");
	if (dir == NULL || dir[0] != '/') {
		fputs("Bad home directory.\n", stderr);
		exit(1);
	}
	dirlen = strlen(dir);

	ooi_path = malloc(dirlen + sizeof("/" INSTANCE_LOCK));
	if (ooi_path == NULL)
		fail("malloc");
	memcpy(ooi_path, dir, dirlen);
	memcpy(ooi_path + dirlen, "/" INSTANCE_LOCK,
	    sizeof("/" INSTANCE_LOCK));  /* copies '\0' */

	fd = open(ooi_path, O_RDWR | O_CREAT, 0600);
	if (fd < 0)
		fail(ooi_path);

	fl.l_start = 0;
	fl.l_len = 0;
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	if (fcntl(fd, F_SETLK, &fl) < 0) {
		fputs("Another instance of this program is running.\n",
		    stderr);
		exit(1);
	}

	/*
	 * Run unlink(ooi_path) when the program exits. The program
	 * always releases locks when it exits.
	 */
	atexit(ooi_unlink);
}

void ayuda();
int compararAccion(char *, char *);
int menuPrincipal(char *);
void mostrarSalida(const char *);

int main(int argc, const char* argv[]) {
   
   if(argc == 2) {
      if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
         ayuda();
         exit(0);
      } else {
         mostrarError();
         exit(0);
      }
   } else if (argc > 2) {
         mostrarError();
         exit(0);
   }

   only_one_instance();

   signal(SIGINT, SIG_IGN); 

   
   int continuar = 1;
   char *mensaje, salida[1024], opcion[25], entrada[40];
   char *FIFO1 = "/tmp/pipe.in", 
        *FIFO2 = "/tmp/pipe.out";

   mkfifo(FIFO1, 0666);
   mkfifo(FIFO2, 0666);

   mensaje = malloc(sizeof(char) * 20);
   
   do {
      switch(menuPrincipal(opcion)) {
         case STOCK:
            strcpy(mensaje, opcion + 6);
            sprintf(entrada, "%c|%s", '1', mensaje);
            writeFifo(FIFO1, entrada);
            memset(salida, '\0', sizeof(char) * 1024);
            readFifo(FIFO2, salida, sizeof(salida));
            mostrarSalida(salida);
            break;
         case SIN_STOCK:
            strcpy(mensaje, opcion + 10);
            sprintf(entrada, "%c|%s", '2', mensaje);
            writeFifo(FIFO1, entrada);
            memset(salida, '\0', sizeof(char) * 1024);
            readFifo(FIFO2, salida, sizeof(salida));
            mostrarSalida(salida);
            break;
         case REPO: 
            strcpy(mensaje, opcion + 5);
            sprintf(entrada, "%c|%s", '3', mensaje);
            writeFifo(FIFO1, entrada);
            memset(salida, '\0', sizeof(char) * 1024);
            readFifo(FIFO2, salida, sizeof(salida));
            mostrarSalida(salida);
            break;
         case LIST: 
            strcpy(mensaje, opcion + 5);
            sprintf(entrada, "%c|%s", '4', mensaje);
            writeFifo(FIFO1, entrada);
            memset(salida, '\0', sizeof(char) * 1024);
            readFifo(FIFO2, salida, sizeof(salida));
            mostrarSalida(salida);
            break;
         case QUIT:
            writeFifo(FIFO1, "5");
            continuar = 0;
            break;
         default:
            break;
      }
   } while (continuar);

   free(mensaje);

   return 0;
}

int menuPrincipal(char *opcion) {
   
	int esInvalido;
	memset(opcion, '\0', sizeof(char) * 25);
	do {
		//yellow();
		printf("\n-------------Seleccione una Opción-------------\n");
		printf(" STOCK producto_id (muestra DESCRIPCION y STOCK para un producto dado.)\n");
		printf(" SIN_STOCK (muestra ID, DESCRIPCION y COSTO de los productos con STOCK cero.)\n");
		printf(" REPO cantidad (Muestra el costo total de reponer una cantidad dada para cada producto sin stock.)\n");
		printf(" LIST (Muestra ID, DESCRIPCION y PRECIO de todos los productos existentes.)\n");
		printf(" QUIT (Finaliza la ejecución.)\n");
		fflush(stdin);
      fgets(opcion, 25, stdin);
		
      esInvalido = 0;
		if ((compararAccion("STOCK", opcion) != 0) 
            && (compararAccion("SIN_STOCK", opcion) != 0) 
            && (compararAccion("REPO", opcion) != 0) 
            && (compararAccion("LIST", opcion) != 0) 
            && (compararAccion("QUIT", opcion) != 0)) {
			system("clear");
         red();
			printf("\n-----------------OPCIÓN INVÁLIDA------------\n");
         reset();
			esInvalido = 1;
		}
	} while (esInvalido);
	if ((compararAccion("STOCK", opcion) == 0)) {
		return STOCK;
	} else if ((compararAccion("SIN_STOCK", opcion) == 0)) {
		return SIN_STOCK;
	} else if ((compararAccion("REPO", opcion) == 0)) {
		return REPO;
	} else if ((compararAccion("LIST", opcion) == 0)) {
		return LIST;
	} else if ((compararAccion("QUIT", opcion) == 0)) {
		return QUIT;
	}
}

int compararAccion(char* s1, char* s2){
	int i=0;

	while (s1[i] != '\0' && s2[i]!=' ' && s2[i]!='\n') {
		if (s1[i] != s2[i]) {
			return -1;
		}
		i++;
	}
   if(strlen(s1) != i) {
      return 1;
   }
	return 0;
}

void mostrarSalida(const char * salida) {
   green();
   puts(salida);
   reset();
}

void ayuda() {
   puts("*************************************************");
   puts("El proceso cliente recibe comandos por teclado.");
   puts("Ejemplo de ejecución del cliente: ./client");
   puts("Luego, el cliente muestra una lista de opciones para");
   puts("poder consultar la informacion de manera automatizada.");
   puts("*************************************************");
}