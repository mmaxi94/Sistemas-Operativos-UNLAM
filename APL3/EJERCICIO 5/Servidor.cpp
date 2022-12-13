/*
    ####################   ENCABEZADO    ##########################
    #							                                  #
    #	Nombre de los archivos: Servidor.cpp	                  #
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

#include <chrono>
#include <csignal>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define TAM_BUFFER 500
#define CANT_CLIENTES_MAX 3

typedef struct {
    char accion[9];
    char nombre[10];
    char raza[10];
    char sexo;
    char castrado[3];
}t_registro;

static int fdServidor = socket(AF_INET, SOCK_STREAM, 0);    

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
    close(fdServidor);
    remove("gatosFile.bin");
    exit(0);
}


using namespace std;

void procesamientoServidor(int fdCliente) {
    char buf[TAM_BUFFER];
    char bufRta[TAM_BUFFER];
    FILE* pfTmp;
    t_registro datos;
    t_registro registroArchivo;
    int numbytes;
    char * finDeCadenaDeAccion;
    char * finDeCadenaDeNombre;
    char * finDeCadenaDeRaza;
    char * finDeCadenaDeCastrado;
    char * finDeCadenaDeSexo;

    int largoAccion;
    int largoNombre;
    int largoRaza;
    int largoCastrado; 
    int largoSexo; 

    numbytes = recv(fdCliente, buf, TAM_BUFFER,0); 
    buf[numbytes] = '\0'; 
    
    finDeCadenaDeAccion = strchr(buf, '\0');
    largoAccion = finDeCadenaDeAccion - buf + 1;                 
    
    finDeCadenaDeNombre = strchr(finDeCadenaDeAccion+1, '\0'); 
    largoNombre = finDeCadenaDeNombre - (finDeCadenaDeAccion+1) + 1;
    finDeCadenaDeRaza = strchr(finDeCadenaDeNombre+1, '\0');
    largoRaza = finDeCadenaDeRaza - (finDeCadenaDeNombre+1) + 1;
    finDeCadenaDeSexo = strchr(finDeCadenaDeRaza+1, '\0');
    largoSexo = 2; 
    finDeCadenaDeCastrado = strchr(finDeCadenaDeSexo+1, '\0');
    largoCastrado = finDeCadenaDeCastrado - (finDeCadenaDeSexo+1) + 1;
    
    memcpy(datos.accion, buf, largoAccion); 
    memcpy(datos.nombre, finDeCadenaDeAccion+1 , largoNombre); 
    memcpy(datos.raza, finDeCadenaDeNombre+1 , largoRaza); 
    datos.sexo = *(finDeCadenaDeRaza+1);
    memcpy(datos.castrado, finDeCadenaDeRaza+3 , largoCastrado); 
    
    FILE *pf = fopen("./gatosFile.bin", "r+b");
    if (!pf) {
        printf("El archivo gatosFile.bin no se pudo abrir.\n");
        exit(1);
    }
    
    
    if (strcmp(datos.accion,"ALTA") == 0) {
        
        fread(&registroArchivo,sizeof(t_registro),1,pf);
        if(feof(pf)){ 
            strcpy(registroArchivo.nombre,"XXXYYYXXX"); 
        } 
        while(!feof(pf) && strcmp(registroArchivo.nombre,datos.nombre) != 0)  {                
            fread(&registroArchivo,sizeof(t_registro),1,pf);
        }
        

        if(strcmp(registroArchivo.nombre,datos.nombre) != 0) {
            
            fseek(pf,0L,SEEK_END);
            fwrite(&datos, sizeof(t_registro), 1, pf);
            send(fdCliente, "Lo dimos de alta\n", 18, 0); 
        } else if (strcmp(registroArchivo.nombre,datos.nombre) == 0) { 
            send(fdCliente, "Gato repetido.\n", 16,0); 
        }

    }else if (strcmp(datos.accion,"BAJA") == 0) {
        bool bajaEncontrada = false;
        pfTmp = fopen("./gatosFile.tmp", "wb");
        fread(&registroArchivo,sizeof(t_registro),1,pf);
        
        while(!feof(pf)) {
            if (strcmp(registroArchivo.nombre,datos.nombre) != 0) { 
                fwrite(&registroArchivo, sizeof(t_registro),1,pfTmp);
            } else {
                bajaEncontrada = true;
            }
            
            fread(&registroArchivo,sizeof(t_registro),1,pf);
        }

        if (!bajaEncontrada) {
            send(fdCliente, "No es posible dar de baja a un gato inexistente en la DB\n", 58,0);
        } else {
            send(fdCliente, "Se dio de baja el gato.\n", 25,0);
        }
        remove("./gatosFile.bin");
        rename("./gatosFile.tmp","./gatosFile.bin");
        fclose(pfTmp);
       
    } else if (strcmp(datos.accion,"CONSULTA") == 0) {
        int i = 1;
        
        if(strlen(datos.nombre) == 0) {
            
            fseek(pf,0L,SEEK_END);
            int cantRegistros = ftell(pf)/sizeof(t_registro);
            fseek(pf,0L,SEEK_SET);
            
            int largoTotalAEnviar = 0;
            char * punteroBufRta = bufRta;
            
            while(cantRegistros > 0) {                   
                fread(&registroArchivo, sizeof(t_registro),1, pf);
                
                int largoNombreRegistro = strlen(registroArchivo.nombre)+1;
                int largoRazaRegistro = strlen(registroArchivo.raza)+1;
                int largoSexoRegistro = 1;
                int largoCastradoRegistro = 3;
               
                memcpy(punteroBufRta, registroArchivo.nombre, largoNombreRegistro);
                memcpy(punteroBufRta+largoNombreRegistro, registroArchivo.raza, largoRazaRegistro);
                *(punteroBufRta+largoNombreRegistro+largoRazaRegistro) = registroArchivo.sexo; 
                memcpy(punteroBufRta+largoNombreRegistro+largoRazaRegistro+1, registroArchivo.castrado, largoCastradoRegistro);
                punteroBufRta += largoNombreRegistro+largoRazaRegistro+1+largoCastradoRegistro; 
                
                largoTotalAEnviar+=largoNombreRegistro+largoRazaRegistro+1+largoCastradoRegistro;
                i++;
                cantRegistros--;
            }
            send(fdCliente, bufRta, largoTotalAEnviar,0);
        } else {
            size_t cantLeida = fread(&registroArchivo, sizeof(t_registro),1, pf);
            
            while(!feof(pf) && strcmp(registroArchivo.nombre, datos.nombre) != 0) {
                fread(&registroArchivo, sizeof(t_registro),1, pf);
            }
            
            if (cantLeida == 0) {
                strcpy(registroArchivo.nombre, "Borrand");
            }
            
            if(strcmp(registroArchivo.nombre, datos.nombre) == 0) {
                char * punteroBufRta = bufRta;
                
                strcpy(datos.nombre, registroArchivo.nombre); 
                strcpy(datos.raza, registroArchivo.raza); 
                datos.sexo = registroArchivo.sexo;
                strcpy(datos.castrado, registroArchivo.castrado);
                int largoNombreAEnviar = strlen(datos.nombre)+1;
                int largoRazaAEnviar = strlen(datos.raza)+1;
                int largoSexoAEnviar = 2;
                int largoCastradoAEnviar = strlen(datos.castrado)+1;
                memcpy(bufRta, datos.nombre, largoNombreAEnviar);
                memcpy(bufRta+largoNombreAEnviar, datos.raza, largoRazaAEnviar);
                *(bufRta+largoNombreAEnviar+largoRazaAEnviar) = datos.sexo;
                memcpy(bufRta+largoNombreAEnviar+largoRazaAEnviar+1, datos.castrado, largoCastradoAEnviar);
                punteroBufRta += largoNombreAEnviar+largoRazaAEnviar+1+largoCastradoAEnviar; //termina apuntando hasta el \0 del castrado
                send(fdCliente, bufRta, largoNombreAEnviar+largoRazaAEnviar+1+largoCastradoAEnviar,0);
            } else {
                send(fdCliente, "No existe el gato en la DB.\n", 29,0);

            }
        }
    } else {
        send(fdCliente, "Debe ingresar una acción válida.\n", 36,0);
    }

    fclose(pf);
    close(fdCliente);  
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

int main (int argc, char* argv[]) {
    
    if(argc == 2) {
        if (strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"--HELP") == 0 || strcmp(argv[1],"-h") == 0 ){
            printf("\nHELP SERVIDOR:\n");
            printf("\nServidor: %s\n", argv[0]);
            printf("Este servidor gestiona una base de datos de refugio de gatos. Se pueden recibir acciones como CONSULTA, BAJA o ALTA de gatos. \n");
            printf("Este servidor no recibe parametros. Se ejecuta de la forma ./Servidor.exe y queda corriendo como demonio.\n");
            exit(0);
        } 
    }

    if(getProcIdByName("Servidor.exe") > 0){
        cout << "Ya hay una instancia en ejecucion" << endl;
        exit(EXIT_FAILURE);
    }

    skeleton_daemon();

    signal(SIGUSR1, handler);
    
    FILE* pf;
    pf = fopen("./gatosFile.bin", "wb");
    fclose(pf);


    int fdCliente, longitud_cliente; 
    char buf[TAM_BUFFER];
    char bufRta[TAM_BUFFER];
    int puerto = 5558;
    struct sockaddr_in server;
    struct sockaddr_in client;
   
    
    memset(&server, '0', sizeof(server)); 

    server.sin_family = AF_INET; 
    server.sin_port = htons(puerto);
    server.sin_addr.s_addr = INADDR_ANY;
    
    
    bind(fdServidor, (struct sockaddr *)&server, sizeof(server)); 
    
    listen(fdServidor, CANT_CLIENTES_MAX); 

    while(true) {
        longitud_cliente = sizeof(struct sockaddr_in);
   
        fdCliente = accept(fdServidor, (struct sockaddr*)NULL, NULL); 
        thread th(procesamientoServidor, fdCliente);
        th.detach();
    }    
    
    close(fdServidor);

    return EXIT_SUCCESS;
}