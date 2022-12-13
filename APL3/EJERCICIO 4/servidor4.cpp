/*
    ####################   ENCABEZADO    ##########################
    #							                                  #
    #	Nombre de los archivos: servidor4.cpp	                  #
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
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
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
#define NombreDeArchivo "archivo.bin"

#define ISRUNNING "\
#!/bin/bash  \n\
 val=$(ps -ef | grep servidor.exe | wc -l) \n\
 exit $val \n\
"

    static sem_t *semRespuestaServidor = sem_open("respuestaServidor", O_CREAT, 0600, 0); 
    static sem_t *semEscribirPedido = sem_open("escribirSolicitud", O_CREAT, 0600, 0); 
    static sem_t *semPodesLeer = sem_open("podesLeer", O_CREAT, 0600, 0); 
    static sem_t *semYaLei = sem_open("yaLei", O_CREAT, 0600, 0); 
    static sem_t *semCantReg = sem_open("cantReg", O_CREAT, 0600, 0); 
    static sem_t *semPedidoCliente = sem_open("pedidoCliente", O_CREAT, 0600, 0); 
    static sem_t *semEscribirMemoria = sem_open("escribirMemoria", O_CREAT, 0600, 0); 

typedef struct {
    char accion[9];
    char nombre[10];
    char raza[10];
    char castrado[3];
    char sexo;
}t_registro;

static void skeleton_daemon()
{
    pid_t pid;

    pid = fork(); 

    if (pid < 0) { 
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { 
        exit(EXIT_SUCCESS);
    }
    
    
    if (setsid() < 0) { 
        exit (EXIT_FAILURE);
    }

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    pid = fork(); 

    if (pid < 0) { 
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { 
        exit(EXIT_SUCCESS);
    }

    umask(0);

    chdir("./");

    return;
}

void handler(int num) {
    sem_close(semPedidoCliente);
    sem_unlink("pedidoCliente");
    sem_close(semEscribirMemoria);
    sem_unlink("escribirMemoria");
    sem_close(semRespuestaServidor);  
    sem_unlink("respuestaServidor");  
    sem_close(semEscribirPedido);
    sem_unlink("escribirSolicitud");
    sem_close(semPodesLeer);
    sem_unlink("podesLeer");
    sem_close(semYaLei);
    sem_unlink("yaLei");
    sem_close(semCantReg);
    sem_unlink("cantReg");
    remove("archivo.bin");
    shm_unlink(NAME_MEMORY);
    exit(0);
}

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

    

    if(argc == 2) {
        if (strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"--HELP") == 0 || strcmp(argv[1],"-h") == 0 ){
            cout<< "Este servidor gestiona una base de datos de refugio de gatos. Se pueden recibir acciones como CONSULTAR, BAJA o ALTA de gatos." <<endl;
            cout<< "Este servidor no recibe parametros. Se ejecuta de la forma ./Servidor.exe y queda corriendo como demonio.." <<endl;
            exit(0);
        } 
    }

    if(getProcIdByName("servidor.exe") > 0){
        cout << "Ya hay una instancia en ejecucion" << endl;
        exit(EXIT_FAILURE);
    }


    skeleton_daemon();

    signal(SIGUSR1, handler);

    int fd;
    FILE* pf;
    FILE* pfTmp;
    
    //Crear Memoria
    fd = shm_open(NAME_MEMORY, O_CREAT | O_RDWR, 00600);
    ftruncate(fd, sizeof(t_registro)*50);
    close(fd);
    //Fin crear memoria

    pf = fopen("./archivo.bin", "wb");
    fclose(pf);

    fd = shm_open(NAME_MEMORY, O_RDWR, 00400); //Le puse read write
    
    t_registro datos_gato;
    t_registro registroArchivo;

    t_registro* ptr;

    //V(escribirMemoria)
    sem_post(semEscribirMemoria); 
    sem_post(semEscribirPedido);

    while(1) {
        //P(pedidoCliente) este debe empezar en 0
        sem_wait(semPedidoCliente);
        //Leo la solicitud del cliente
        ptr = (t_registro*)mmap(NULL, sizeof(t_registro)*50, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0); //Le cambie los permisos

        strcpy(datos_gato.accion, ptr->accion);
        strcpy(datos_gato.nombre, ptr->nombre);
        strcpy(datos_gato.raza, ptr->raza);
        strcpy(datos_gato.castrado, ptr->castrado);
        datos_gato.sexo = ptr->sexo;

        sem_post(semEscribirMemoria);
        
        pf = fopen("./archivo.bin", "r+b");
    
        if (!pf) {
            printf("El archivo %s no se pudo abrir.\n", NombreDeArchivo);
            return 1;
        }

        //fseek(pf,0,SEEK_CUR);//Siempre hay que hacer esto si voy a leer despues de una escritura.

        if (strcmp(ptr->accion, "ALTA") == 0) {
            //Dar de alta en el archivo. Si ya existe, avisar en memoria compartida al cliente.
            //fseek(pf,0,SEEK_SET);

            fread(&registroArchivo,sizeof(t_registro),1,pf);
            if(feof(pf)){ //Si esta vacio el archivo apenas lo leo, limpio registro archivo.
                strcpy(registroArchivo.nombre,"XXXYYYXXX"); //Con esto soluciono el problema de que si no lei nada, tenga el ultimo registro leido.
                //Y al tener el ultimo registro leido, no podia dar de baja y luego dar de alta
            }

            while(!feof(pf) && strcmp(registroArchivo.nombre,datos_gato.nombre) != 0)  {                
                fread(&registroArchivo,sizeof(t_registro),1,pf);
            }

            //Si es fin de archivo, no encontro el nombre por lo tanto lo da de alta.
            if(strcmp(registroArchivo.nombre,datos_gato.nombre) != 0) {
                fseek(pf,0L,SEEK_END);
                fwrite(&datos_gato, sizeof(t_registro), 1, pf);
            } else if (strcmp(registroArchivo.nombre,datos_gato.nombre) == 0) { 
                sem_wait(semEscribirMemoria);
                strcpy(ptr->accion, "REPETIDO"); 
                sem_post(semEscribirMemoria);
            }      

        } else if (strcmp(ptr->accion, "BAJA") == 0) {
            bool bajaEncontrada;
            bajaEncontrada= false;
            pfTmp = fopen("./archivo.tmp", "wb");

            fread(&registroArchivo,sizeof(t_registro),1,pf);
            while(!feof(pf)) {
                if (strcmp(registroArchivo.nombre,datos_gato.nombre) != 0) { 
                    fwrite(&registroArchivo, sizeof(t_registro),1,pfTmp);
                } else {
                    bajaEncontrada = true;
                }
                
                fread(&registroArchivo,sizeof(t_registro),1,pf);
            }
            if (!bajaEncontrada) {
                sem_wait(semEscribirMemoria);
                strcpy(ptr->accion, "NOTDB");
                sem_post(semEscribirMemoria);
            } else {
                sem_wait(semEscribirMemoria);
                strcpy(ptr->accion, "XDXDXD");
                sem_post(semEscribirMemoria);
            }

            remove("./archivo.bin");
            rename("./archivo.tmp","./archivo.bin");
            fclose(pfTmp);

        } else if (strcmp(ptr->accion, "CONSULTA") == 0) {
            int i = 0;
            if(strcmp(ptr->nombre,"NOMBRE") == 0) { 
                fseek(pf,0L,SEEK_END);
                int cantRegistros = ftell(pf)/sizeof(t_registro);
                fseek(pf,0L,SEEK_SET);

                for (int i = 0; i < cantRegistros; i++) {
                    sem_post(semCantReg);
                }
                
                while(cantRegistros > 0) {
                    fread(&registroArchivo, sizeof(t_registro),1, pf);
                    strcpy(registroArchivo.accion, "LOOP");
                    sem_wait(semEscribirMemoria);
                    memcpy(ptr+(i*sizeof(t_registro)), &registroArchivo, sizeof(t_registro));
                    sem_post(semEscribirMemoria);
                    sem_post(semPodesLeer);
                    i++;
                    cantRegistros--;
                }
            } else {
                size_t cantLeida = fread(&registroArchivo, sizeof(t_registro),1, pf);
                while(!feof(pf) && strcmp(registroArchivo.nombre, ptr->nombre) != 0) {
                    fread(&registroArchivo, sizeof(t_registro),1, pf);
                }
                
                if (cantLeida == 0) {
                    strcpy(registroArchivo.nombre, "Borrand");
                }

                if(strcmp(registroArchivo.nombre, ptr->nombre) == 0) {
                    sem_wait(semEscribirMemoria);

                    strcpy(ptr->accion, "ENCONTRE");
                    strcpy(ptr->nombre, registroArchivo.nombre); 
                    strcpy(ptr->raza, registroArchivo.raza); 
                    ptr->sexo = registroArchivo.sexo;
                    strcpy(ptr->castrado, registroArchivo.castrado);

                    sem_post(semEscribirMemoria);
                } else {
                    sem_wait(semEscribirMemoria);
                    strcpy(ptr->accion, "NOEXISTE");
                    strcpy(ptr->nombre, "NOEXISTE");
                    sem_post(semEscribirMemoria);
                }

            }
        } else {
                sem_wait(semEscribirMemoria);
                strcpy(ptr->accion, "INVALID"); 
                sem_post(semEscribirMemoria);
        }

        sem_post(semRespuestaServidor);

        sem_post(semEscribirPedido);
        fclose(pf);

    }

    close(fd);
    return EXIT_SUCCESS;
}