/*
    ####################   ENCABEZADO    ##########################
    #							          #
    #	Nombre de los archivos: Ejercicio2.cpp	                  #
    #	Numero de APL : 3				                          #
    #	Numero de Ejercicio: 2		                 		      #
    #							                                  #
    #	Integrantes:								              #
    #  		Rodriguez, Cesar Daniel		39166725	              #
    # 		Bastante, Javier 		38621360	              #
    #  		Garcia Velez, Kevin 		38619312	              #
    # 		Morales, Maximiliano 		38176604	              #
    #		Baranda, Leonardo 		36875068	              #
    #                                                             #
    ###############################################################
*/
#include <thread>        
#include <mutex>         
#include <dirent.h>      
#include <fstream>       
#include <semaphore.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>    
#include <cstring>       
#include <sys/inotify.h> 
#include <chrono>        



using namespace std;

void ayuda()
{
        cout << "Bienvenido a la ayuda\n" << endl;
        cout << "Este programa monitorea las modificaciones que se realizan en un directorio y dentro\n" << endl;
        cout << "de sus subdirectorios, dejando un registro de dichos cambios en un log\n" << endl;
        cout << "Ejemplo de ejecucion:\n" << endl;
        cout << "En una terminal: $ ./Ejercicio2 /mnt/c/Sistemas_operativos/APL/APL3/ejercicio2C++\n" << endl;
        cout << "NOTA: se debe proporcionar el path absoluto de un directorio\n" << endl;
        cout << "Para ver la ayuda, ejecute ./Ejercicio2 --help o ./Ejercicio2 -h\n" << endl;
        exit(EXIT_SUCCESS);
}

ofstream outputFile;
string vectorDirectorios[500];
char vectorDirectoriosAUX[128] = {0};
int flagNuevoDirectorio = 0;
pthread_t vectorHilos[500];
char vectorDirectoriosEliminar[128] = {0};
int flagEliminaDirectorio = 0;
pthread_mutex_t mutexSalida = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexNewDir = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexEliminaThread = PTHREAD_MUTEX_INITIALIZER;





void listDir(const char *nombreDir, string *vectorDirectorios, int *cantDir)
{
        struct dirent *entry;
        DIR *dir = opendir(nombreDir);
        entry = readdir(dir);

        while (entry != NULL)
        {
                if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                        char ruta[128] = {0};
                        strcat(ruta, nombreDir);
                        strcat(ruta, "/");
                        strcat(ruta, entry->d_name);
                        vectorDirectorios[(*cantDir)] = ruta;
                        (*cantDir)++;
                        listDir(ruta, vectorDirectorios, cantDir);
                }
                entry = readdir(dir);
        }
        closedir(dir);
}

int existeDirectorio(char path[])
{
        struct stat fileAux;

        if (stat(path, &fileAux) == 0)
        {
                if (S_ISDIR(fileAux.st_mode))
                {
                        return 0;
                }
        }
        return 1;
}

void *monitorear(void *arg)
{
        int index = *(int *)arg;
        char buffer[200];
        char pdirect[128];
        int fd, wd;

        vectorHilos[index] = pthread_self();
        strcpy(pdirect, vectorDirectorios[index].c_str());
        fd = inotify_init();
        wd = inotify_add_watch(fd, pdirect, IN_MODIFY | IN_CREATE | IN_DELETE);

        while (true)
        {
                read(fd, buffer, 200);


                if (((struct inotify_event *)buffer)->mask & IN_MODIFY)
                {

                        if (((struct inotify_event *)buffer)->mask & IN_ISDIR)
                        {
                        pthread_mutex_lock(&mutexSalida);
                        cout << "The directory " << ((struct inotify_event *)buffer)->name << " was modified on " << vectorDirectorios[index] << endl;
                        outputFile << "The directory " << ((struct inotify_event *)buffer)->name << " was modified on " << vectorDirectorios[index] << endl;
                        pthread_mutex_unlock(&mutexSalida);
                        }
                        else
                        {
                        pthread_mutex_lock(&mutexSalida);
                        cout << "The file " << ((struct inotify_event *)buffer)->name << " was modified on " << vectorDirectorios[index] << endl;
                        outputFile << "The file " << ((struct inotify_event *)buffer)->name << " was modified on " << vectorDirectorios[index] << endl;  
                        pthread_mutex_unlock(&mutexSalida);
                        }
                }

                if (((struct inotify_event *)buffer)->mask & IN_CREATE)
                {
                        if (((struct inotify_event *)buffer)->mask & IN_ISDIR)
                        {
                                pthread_mutex_lock(&mutexNewDir);
                                for (int j = 0; j < 128; j++)
                                {
                                        vectorDirectoriosAUX[j] = {0};
                                }
                                strcat(vectorDirectoriosAUX, pdirect);
                                strcat(vectorDirectoriosAUX, "/");
                                strcat(vectorDirectoriosAUX, ((struct inotify_event *)buffer)->name);
                                flagNuevoDirectorio++;
                                cout << "The directory " << ((struct inotify_event *)buffer)->name << " was created on " << vectorDirectoriosAUX << endl;
                                outputFile << "The directory " << ((struct inotify_event *)buffer)->name << " was created on " << vectorDirectoriosAUX << endl;
                                pthread_mutex_unlock(&mutexNewDir);
                        }
                        else
                        {
                                pthread_mutex_lock(&mutexSalida);
                                cout << "The file " << ((struct inotify_event *)buffer)->name << " was created on " << vectorDirectorios[index] << endl;
                                outputFile << "The file " << ((struct inotify_event *)buffer)->name << " was created on " << vectorDirectorios[index] << endl;
                                pthread_mutex_unlock(&mutexSalida);
                        }
                }

                if (((struct inotify_event *)buffer)->mask & IN_DELETE)
                {
                        if (((struct inotify_event *)buffer)->mask & IN_ISDIR)
                        {
                                pthread_mutex_lock(&mutexEliminaThread);
                                for (int k = 0; k < 128; k++)
                                {
                                        vectorDirectoriosEliminar[k] = {0};
                                }
                                strcat(vectorDirectoriosEliminar, pdirect);
                                strcat(vectorDirectoriosEliminar, "/");
                                strcat(vectorDirectoriosEliminar, ((struct inotify_event *)buffer)->name);
                                flagEliminaDirectorio++;
                                cout << "The directory " << ((struct inotify_event *)buffer)->name << " was deleted on " << vectorDirectoriosEliminar << endl;
                                outputFile << "The directory " << ((struct inotify_event *)buffer)->name << " was deleted on " << vectorDirectoriosEliminar << endl;
                                pthread_mutex_unlock(&mutexEliminaThread);
                        }
                        else
                        {
                                pthread_mutex_lock(&mutexSalida);
                                cout << "The file " << ((struct inotify_event *)buffer)->name << " was deleted on " << vectorDirectorios[index] << endl;
                                outputFile << "The file " << ((struct inotify_event *)buffer)->name << " was deleted on " << vectorDirectorios[index] << endl;
                                pthread_mutex_unlock(&mutexSalida);
                        }
                }
        }
        free(arg);
        return NULL;
}

int main(int argc, char *argv[])
{
        char *pathEntrada;

        if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
        {
                ayuda();
                return 0;
        }

        if ((argc > 2) || (argc < 2))
        {
                ayuda();
                return 0;
        }


        if (existeDirectorio(argv[1]) == 0)
        {
                if ((strcmp(argv[1], ".") == 0))
                {
                        char cwd[10000];
                        getcwd(cwd, sizeof(cwd));
                        pathEntrada = cwd;
                }
                else
                        pathEntrada = argv[1];
        }
        else
        {
                cout << "path invalido" << endl;
                return 0;
        }

        //cout << pathEntrada << endl;

        int i, cantDir = 1;

        outputFile.open("/tmp/logFile.txt", ios::out);
        if (outputFile.fail())
        {
                cout << "Error al crear el archivo de log." << endl;
                exit(1);
        }

        vectorDirectorios[0] = pathEntrada;
        listDir(pathEntrada, vectorDirectorios, &cantDir);

        pthread_t hilos[cantDir];

        cout << "rutas a monitorear:" << endl;
        for (i = 0; i < cantDir; i++)
        {
                cout << vectorDirectorios[i] << endl;
                int *a = (int *)malloc(sizeof(int));
                *a = i;
                if (pthread_create(&hilos[i], NULL, &monitorear, a) != 0)
                {
                        cout << "Error al crear hilos." << endl;
                }
        }

        while (true)
        {
                pthread_mutex_lock(&mutexNewDir);
                if (flagNuevoDirectorio != 0)
                {
                        cantDir++;
                        i++;
                        int *b = (int *)malloc(sizeof(int));
                        *b = i;
                        vectorDirectorios[i] = vectorDirectoriosAUX;
                        pthread_t nuevoHilo;
                        pthread_create(&nuevoHilo, NULL, &monitorear, b);
                        flagNuevoDirectorio--;
                }
                pthread_mutex_unlock(&mutexNewDir);

                pthread_mutex_lock(&mutexEliminaThread);
                if (flagEliminaDirectorio != 0)
                {
                        cantDir--;
                        pid_t auxIndex = 0;
                        while (vectorDirectorios[auxIndex] != vectorDirectoriosEliminar)
                        {
                                auxIndex++;
                        }
                        pthread_cancel(vectorHilos[auxIndex]);
                        vectorDirectorios[auxIndex] = "n";
                        vectorHilos[auxIndex] = 0;
                        flagEliminaDirectorio--;
                }
                pthread_mutex_unlock(&mutexEliminaThread);
        }

        for (i = 0; i < cantDir; i++)
        {
                if (pthread_join(hilos[i], NULL) != 0)
                {
                        cout << "Error al esperar hilos." << endl;
                }
        }

        outputFile.close();



        pthread_mutex_destroy(&mutexSalida);
        pthread_mutex_destroy(&mutexNewDir);
        pthread_mutex_destroy(&mutexEliminaThread);

        return 0;
}