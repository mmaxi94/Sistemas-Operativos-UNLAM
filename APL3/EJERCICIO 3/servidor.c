#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <signal.h>

#include "fifo.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define STOCK 1
#define SIN_STOCK 2
#define REPO 3
#define LIST 4
#define QUIT 5



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

typedef struct {
   int id;
   char descripcion[50];
   int precio;
   int costo;
   int stock;
} Producto;

typedef struct {
	char accion;
	char parametro[20];
} Parametros;

void ayuda();
void validarArchivo(const char *);
void stringToParam(char *, Parametros *);
int parsearProducto(char *, Producto *);
int mostrarProducto(char *, char *, int);
int mostrarProductosSinStock(char *, char *);
int calcularReposicion(char *, char *, int);
int listarTodosLosProductos(char *, char *);





void sigint_handler(int sig)
{
   fprintf(stderr, "Caught signal %d.\n", sig);
   unlink("/tmp/MyUniqueName");
   /* exit() is not safe in a signal handler, use _exit() */
   _exit(1);
}

int main(int argc, char const *argv[]) {

   signal(SIGINT, SIG_IGN); 
   if(argc != 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      ayuda();
      exit(0);
   }

      

   if (argc == 2) {
      validarArchivo(argv[1]);
   }

   struct sigaction act;
   int myfd;
   
   myfd = open("/tmp/MyUniqueName", O_CREAT|O_EXCL);
   if ( myfd < 0 )
   {
      fprintf(stderr, "I am already running!\n");
      exit(1);
   }
   act.sa_handler = sigint_handler;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   sigaction(SIGINT, &act, NULL);

   char *FIFO1 = "/tmp/pipe.in", 
        *FIFO2 = "/tmp/pipe.out",
        msj[1024], 
        pathFile[200], 
        mensaje[40];
   int continuar = 1;
   Parametros datos;

   strcpy(pathFile, argv[1]);

   switch (fork()) {
      case -1: return -1;
      case 0: break;
      default: _exit(EXIT_SUCCESS);
   }

   if(setsid() == -1)
      return -1;

   switch(fork()) {
    case -1: return -1;
    case 0: break;
    default: _exit(EXIT_SUCCESS);
  }
   
   do {
      memset(mensaje, '\0', 20);
      readFifo(FIFO1, mensaje, sizeof(mensaje));
      stringToParam(mensaje, &datos);
      switch (datos.accion) {
      case '1':
         memset(msj, '\0', sizeof(msj));
         mostrarProducto(pathFile, msj, atoi(datos.parametro));
         writeFifo(FIFO2, msj);
         break;
      case '2':
         memset(msj, '\0', sizeof(msj));
         mostrarProductosSinStock(pathFile, msj);
         writeFifo(FIFO2, msj);
         break;
      case '3':
         memset(msj, '\0', sizeof(msj));
         calcularReposicion(pathFile, msj, atoi(datos.parametro));
         writeFifo(FIFO2, msj);
         break;
      case '4':
         memset(msj, '\0', sizeof(msj));
         listarTodosLosProductos(pathFile, msj);
         writeFifo(FIFO2, msj);
         break;
      case '5':
         //Finalizacion de la ejecucion.
         continuar = 0;
         break;
      default:
         break;
      }
   } while (continuar);

   unlink(FIFO1);
   unlink(FIFO2);
   unlink("/tmp/MyUniqueName"); 
   close(myfd);

   return 0;
}

int parsearProducto(char * linea, Producto * producto) {
   char * aux = strchr(linea, '\n');
   aux = strrchr(linea, ';');
   //STOCK
   *aux = '\0';
   aux = strrchr(linea, ';');
   sscanf(aux+1, "%d", &producto->stock);
   //COSTO
   *aux = '\0';
   aux = strrchr(linea, ';');
   sscanf(aux+1, "%d", &producto->costo);
   //PRECIO
   *aux = '\0';
   aux = strrchr(linea, ';');
   sscanf(aux+1, "%d", &producto->precio);
   //DESCRIPCION
   *aux = '\0';
   aux = strrchr(linea, ';');
   strncpy(producto->descripcion, aux+1, sizeof(producto->descripcion));
   //ID
   *aux = '\0';
   sscanf(linea, "%d", &producto->id);
}

int mostrarProducto(char * path, char * mensaje, int productoId) {
   char linea[200];
   int i = 0;
   FILE * fp;
   Producto producto;

   fp = fopen(path,"rt");
   if(fp == NULL) {
      return -1;
   }
   //Leo cabecera
   fgets(linea, 200, fp);
   while(fgets(linea, 200, fp)) {
      parsearProducto(linea, &producto);
      if (producto.id == productoId) {
         sprintf(mensaje, "%s %du", producto.descripcion, producto.stock);
      }
   }

   fclose(fp);
   return 0;
}

int mostrarProductosSinStock(char * path, char * mensaje) {
   char linea[200];
   FILE * fp;
   Producto producto;
   char productoSinStock[200];

   fp = fopen(path,"rt");
   if(fp == NULL) {
      return -1;
   }
   //Leo cabecera
   fgets(linea, 200, fp);
   while(fgets(linea, 200, fp)) {
      parsearProducto(linea, &producto);
      if (producto.stock == 0) {
         sprintf(productoSinStock, "%d %s $%d\n", producto.id, producto.descripcion, producto.costo);
         strcat(mensaje, productoSinStock);
      }
   }

   fclose(fp);
   return 0;
}

int calcularReposicion(char * path, char * mensaje, int cantidadAReponer) {
   char linea[200];
   FILE * fp;
   Producto producto;
   int totalAReponer = 0;

   fp = fopen(path,"rt");
   if(fp == NULL) {
      return -1;
   }
   //Leo cabecera
   fgets(linea, 200, fp);
   while(fgets(linea, 200, fp)) {
      parsearProducto(linea, &producto);
      if (producto.stock == 0) {
         totalAReponer += cantidadAReponer * producto.costo;
      }
   }

   sprintf(mensaje, "$%d", totalAReponer);

   fclose(fp);
   return 0;
}

int listarTodosLosProductos(char * path, char * mensaje) {
   char linea[200];
   FILE * fp;
   Producto producto;
   char detalleProducto[200];

   fp = fopen(path,"rt");
   if(fp == NULL) {
      return -1;
   }
   //Leo cabecera
   fgets(linea, 200, fp);

   while(fgets(linea, 200, fp)) {
      parsearProducto(linea, &producto);
      sprintf(detalleProducto, "%d %s $%d\n", producto.id, producto.descripcion, producto.precio);
      strcat(mensaje, detalleProducto);
   }

   fclose(fp);
   return 0;
}

void ayuda() {
   puts("Debe ingresar como parametro el archivo de productos a analizar.");
   puts("Ejemplo de ejecución de servidor: ./server productos.txt");
   puts("Luego, el servidor quedará ejecutandose en segundo plano escuchando las peticiones del cliente.");
}

void stringToParam(char *mensaje, Parametros *datos){
   datos->accion = mensaje[0];
   memset(datos->parametro, '\0', 20);
   strcpy(datos->parametro, mensaje + 2);
}

void validarArchivo(const char * path) {
   if (access(path, F_OK) != 0) {
      printf("el archivo %s no existe. Para mas informacion, utilice el parametro -h o --help\n", path);
      exit(-2);
   } 
}