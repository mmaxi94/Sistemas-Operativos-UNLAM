/*
	####################   ENCABEZADO    ##########################
	#							                                  #
	#	Nombre de los archivos: cliente4.cpp	                  #
	#	Numero de APL : 3				                          #
	#	Numero de Ejercicio: 4		                 		      #
	#							                                  #
	#	Integrantes:								              #
	#  		Rodriguez, Cesar Daniel		39166725	              #
	#  		Garcia Velez, Kevin 		38619312	              #
	# 		Morales, Maximiliano 		38176604	              #
	#                                                             #
	###############################################################
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <chrono>
#include <thread>
#include <fcntl.h>
#include <filesystem>
#include <semaphore.h>
#include <sys/stat.h>
#include <cstring>
#include <csignal>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>

using namespace std;

#define NAME_MEMORY "MemoriaCompartidaSC"
#define ISRUNNING "\
#!/bin/bash  \n\
 val=$(ps -ef | grep cliente.exe | wc -l) \n\
 exit $val \n\
"

void help() {
    cout<<"AYUDA:"<< endl;
    cout << "Ejecute de la siguiente manera: " << endl;
    cout << "para dar de alta a 1 gato ejecute-->       ./cliente.exe ALTA [NOMBRE] [RAZA] [sexo(H|M)] [castrado(CA)|sinCastrar(SC)]" << endl;
    cout << "para dar de baja a un gato ejecute-->      ./cliente.exe BAJA [NOMBRE]" << endl;
    cout << "para consultar en los registros ejecute--> ./cliente.exe CONSULTA [NOMBRE(ES OPCIONAL)]" << endl; 

    cout << "INFO ADICIONAL: PARA DAR DE ALTA: ESCRIBA CA SI ESTA CASTRADO Y SC SI NO LO ESTA "<< endl;
    cout << "NOMBRE DEL GATO: SE PERMITE UN MAXIMO DE 10 CARACTERES PARA EL NOMBRE "<< endl;
    cout << "NOMBRE DE LA RAZA: SE PERMITE UN MAXIMO DE 10 CARACTERES PARA EL NOMBRE DE LA RAZA "<< endl;
}

typedef struct {
    char accion[9];//ALTA,CONSULTA,BAJA
    char nombre[10];
    char raza[10];
    char castrado[3];
    char sexo;
}t_registro;

int getProcIdByName(string procName)
{
    int currPid = ::getpid();
    int pid = -1;
    // Open the /proc directory
    DIR *dp = opendir("/proc");
    if (dp != NULL)
    {
        // Enumerate all entries in directory until process found
        struct dirent *dirp;
        while (pid < 0 && (dirp = readdir(dp)))
        {
            // Skip non-numeric entries
            int id = atoi(dirp->d_name);
            if (id > 0)
            {
                // Read contents of virtual /proc/{pid}/cmdline file
                string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
                ifstream cmdFile(cmdPath.c_str());
                string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty())
                {
                    // Keep first cmdline item which contains the program path
                    size_t pos = cmdLine.find('\0');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(0, pos);
                    // Keep program name only, removing the path
                    pos = cmdLine.rfind('/');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(pos + 1);
                    // Compare against requested process name
                    if (procName == cmdLine && id != currPid)
                        pid = id;
                }
            }
        }
       return pid;
    }

    closedir(dp);

    return pid;
}

int main(int argc, char* argv[]) {
    

    if(getProcIdByName("cliente.exe") > 0){
        cout << "Ya hay una instancia en ejecucion" << endl;
        exit(EXIT_FAILURE);
    }

    if (argc < 2 || argc > 6) {
        cout << "INGRESASTE UN NUMERO INCORRECTO DE PARAMETROS "<< endl;
        help();
        exit(1);
    }

    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "--help") == 0))
    {
        help();
        exit(0);
    }
    
    //INICIALIZO LOS SEMAFOROS
    sem_t *semRespuestaServidor = sem_open("respuestaServidor", O_CREAT, 0600, 0); 
    sem_t *semEscribirPedido = sem_open("escribirSolicitud", O_CREAT, 0600, 0); 
    sem_t *semPodesLeer = sem_open("podesLeer", O_CREAT, 0600, 0); 
    sem_t *semYaLei = sem_open("yaLei", O_CREAT, 0600, 0); 
    sem_t *semCantReg = sem_open("cantReg", O_CREAT, 0600, 0); 
    sem_t *semPedidoCliente = sem_open("pedidoCliente", O_CREAT, 0600, 0);
    sem_t *semEscribirMemoria = sem_open("escribirMemoria", O_CREAT, 0600, 0); 

    int fd;
    fd = shm_open(NAME_MEMORY, O_RDWR, 0);
    ftruncate(fd, (long long)((long long)sizeof(t_registro)*(long long)50));

    t_registro* ptr;
    t_registro datos_gato;
    
    

    ptr = (t_registro*)mmap(0, sizeof(t_registro), PROT_WRITE, MAP_SHARED, fd, 0);

    if (strcmp(argv[1],"ALTA") == 0) {

        if (argc < 6) {
            cout << "INGRESO INCORRECTO"<< endl;
            help();
            exit(3);
        }

        //ACA VALIDO QUE ESTOY MANDANDO BIEN LOS PARAMETROS DE ALTA
        if ( !strchr(argv[2],0) || !strchr(argv[3],0) || (*(argv[4]) != 'M' && *(argv[4]) != 'H') || !strchr(argv[5],0) || 
            strlen(argv[2]) == 1 || strlen(argv[3]) == 1 || (strcmp(argv[5],"CA") != 0 && strcmp(argv[5],"SC") != 0) ) {
            cout << "INGRESO INCORRECTO"<< endl;
            help();
            exit(3);
        }
        sem_wait(semEscribirPedido);

        //EXTRAIGO LOS DATOS PARA DAR DE ALTA,CONSULTAR O DAR DE BAJA
        strcpy(datos_gato.accion, argv[1]);
        strcpy(datos_gato.nombre, argv[2]);
        strcpy(datos_gato.raza, argv[3]);
        datos_gato.sexo = *argv[4];
        strcpy(datos_gato.castrado,argv[5]);

        //PASO LOS DATOS A LA MEMORIA COMPARTIDA
        memcpy(ptr, &datos_gato, sizeof(t_registro)); //Escribo en memoria
    } else if (strcmp(argv[1],"BAJA") == 0) {
        
        if (argc < 3 ) {
            cout << "INGRESO INCORRECTO"<< endl;
            help();
            exit(3);
        }

        if (strchr(argv[2],'\0') == NULL || strlen(argv[2]) == 1) {
            cout << "INGRESO INCORRECTO"<< endl;
            help();
            exit(3);
        }
        sem_wait(semEscribirPedido);

        strcpy(datos_gato.accion, argv[1]);
        strcpy(datos_gato.nombre, argv[2]);

        memcpy(ptr, &datos_gato, sizeof(t_registro)); 
    } else if (strcmp(argv[1],"CONSULTA") == 0) {

        if ( argc == 3 && (strchr(argv[2],'\0') == NULL || strlen(argv[2]) == 1) ) {
            help();
            exit(3);
        }
        sem_wait(semEscribirPedido);
        strcpy(datos_gato.accion, argv[1]);
        if(argc == 3) {
            strcpy(datos_gato.nombre, argv[2]);
        } else {
            strcpy(datos_gato.nombre, "NOMBRE");
        }
        
        sem_wait(semEscribirMemoria);
        strcpy(ptr->accion, datos_gato.accion);
        strcpy(ptr->nombre, datos_gato.nombre);
        //memcpy(ptr, &datos_gato, sizeof(t_registro)); //Escribo el registro entero en el puntero (probar hacerlo uno por uno si no funciona algo)
        sem_post(semEscribirMemoria);
    } else {
        cout << "INGRESA ALGUNA DE LAS SIGUIENTES CONSULTAS: ALTA|BAJA|CONSULTA"<< endl;
        help();
    }
    

    sem_post(semPedidoCliente);
    sem_wait(semRespuestaServidor);

    ptr = (t_registro*)mmap(NULL, sizeof(t_registro), PROT_READ, MAP_SHARED, fd, 0);
   
    if(strcmp(ptr->accion, "LOOP") == 0) { 
      
        ptr = (t_registro*)mmap(NULL,(long long) sizeof(t_registro)*50, PROT_READ, MAP_SHARED, fd, 0);

        int valor;
        sem_getvalue(semCantReg,&valor);

        t_registro registro;

        for (int i = 0; i < valor; i++) {
            sem_wait(semPodesLeer);
            memcpy(&registro,ptr+((long long)((long long)sizeof(t_registro)*(long long)i)), sizeof(t_registro));
            printf("Nombre de gato: %s | Raza: %s | Sexo: %c | Castrado: %s\n", registro.nombre, registro.raza, registro.sexo, registro.castrado);
            sem_wait(semCantReg);
        }
    }

    if(strcmp(ptr->accion, "ENCONTRE") == 0) { 
        printf("Datos del gato: \n");
        printf("Nombre de gato: %s | Raza: %s | Sexo: %c | Castrado: %s\n", ptr->nombre, ptr->raza, ptr->sexo, ptr->castrado);
    }

    if(strcmp(ptr->accion, "NOEXISTE") == 0) { 
        cout << "El gato que buscas no existe"<< endl;
    }
    
    if(strcmp(ptr->accion, "NOTDB") == 0) { 
        cout << "El gato que queres dar de baja no existe en nuestros registros"<< endl;
    }

    if(strcmp(ptr->accion, "REPETIDO") == 0) { 
        cout << "El gato que queres ingresar ya fue ingresado al sistema"<< endl;
    }
    
    if(strcmp(ptr->accion, "INVALID") == 0) { 
        cout << "INGRESA ALGUNA DE LAS SIGUIENTES CONSULTAS: ALTA|BAJA|CONSULTA"<< endl;
    }

    close(fd);

    //Cerramos los semaforos usados
    sem_close(semPedidoCliente);
    sem_close(semEscribirMemoria);
    sem_close(semRespuestaServidor);  
    sem_close(semEscribirPedido);
    sem_close(semPodesLeer);
    sem_close(semYaLei);
    sem_close(semCantReg);

    return EXIT_SUCCESS;
}
