#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

void ayuda();
/*
    ####################   ENCABEZADO    ##########################
    #							                                  #
    #	Nombre de los archivos: ejercicio1.c	                  #
    #	Numero de APL : 3				                          #
    #	Numero de Ejercicio: 1		                 		      #
    #							                                  #
    #	Integrantes:								              #
    #  		Rodriguez, Cesar Daniel		39166725	              #
    # 		Bastante, Javier 			38621360	              #
    #  		Garcia Velez, Kevin 		38619312	              #
    # 		Morales, Maximiliano 		38176604	              #
    #		Baranda, Leonardo 			36875068	              #
    #                                                             #
    ###############################################################
*/

void ayuda()
{
    printf("Bienvenido a la ayuda\n");
    printf("Este programa genera mediante el uso de fork, na jerarquía de hijos, nietos, y bisnietos.\n");
    printf("Algunos procesos quedaran en estado zombie o en estado demonio.\n");
    printf("\n");
    printf("Ejemplo de ejecucion:\n");
    printf("En una terminal: $ ./ejercicio1\n");
    printf("\n");
    printf("Para ver la ayuda, ejecute ./ejercicio1 -help\n");
    exit(EXIT_SUCCESS);
}

void mostrarInfo(int pid, int generacion, int ppid, char *parentesco, char *tipo)
{
    printf("Soy el proceso con PID: %d y pertenezco a la generación Nº: %d - PPID Padre: %d Parentesco-Tipo: %s - %s\n\n", pid, generacion, ppid, parentesco, tipo);
}

void error()
{
    printf("ERROR. Falló la creacion del proceso. \n\n");
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
        {
            ayuda();
            exit(0);
        }
        else
        {
            ayuda();
            exit(0);
        }
    }
    else
    {

        // Guardamos los PIDS de los hijos
        int pid = fork();

        if (pid > 0) // padre
        {
            mostrarInfo(getpid(), 0, getppid(), "padre", "Normal");

            pid = fork();

            if (pid > 0)
            {
                int status;
                pid_t w = waitpid(pid, &status, WUNTRACED);
            }
            if (pid == 0) // hijo2
            {
                mostrarInfo(getpid(), 1, getppid(), "segundo hijo", "Normal");

                pid = fork();

                if (pid > 0)
                {
                    int status;
                    pid_t w = waitpid(pid, &status, WUNTRACED);
                }
                if (pid == 0)
                {
                    mostrarInfo(getpid(), 2, getppid(), "primer nieto", "Normal");

                    pid = fork();
                    if (pid > 0)
                    {
                        int status;
                        pid_t w = waitpid(pid, &status, WUNTRACED);
                    }
                    else
                    {
                        mostrarInfo(getpid(), 3, getppid(), "primer bisnieto", "Normal");
                    }
                }
            }
        }
        else if (pid == 0) // hijo1
        {
            mostrarInfo(getpid(), 1, getppid(), "primer hijo", "Normal");

            int pid = fork();

            if (pid > 0)
            {
                pid = fork();

                if (pid > 0)
                {
                    int status;
                    pid_t w = waitpid(pid, &status, WUNTRACED);
                }
                else if (pid == 0)
                {
                    mostrarInfo(getpid(), 2, getppid(), "segundo nieto", "Normal");

                    pid = fork();
                    if (pid > 0)
                    {
                        pid = fork();

                        if (pid > 0)
                        {
                            int status;
                            pid_t w = waitpid(pid, &status, WUNTRACED);
                        }
                        else if (pid == 0)
                        {
                            mostrarInfo(getpid(), 3, getppid(), "segundo bisnieto", "Normal");
                        }
                        else if (pid == 0)
                        {
                            mostrarInfo(getpid(), 3, getppid(), "tercer bisnieto", "Normal");
                        }
                    }
                }
            }

            else if (pid == 0)
            {
                mostrarInfo(getpid(), 2, getppid(), "tercer nieto", "Normal");

                pid = fork();
                if (pid > 0)
                {
                    pid = fork();

                    if (pid > 0)
                    {
                        while (1)
                        {
                            /* code */
                        }
                    }
                    else if (pid == 0)
                    {
                        mostrarInfo(getpid(), 3, getppid(), "cuarto bisnieto", "Zombie");
                        exit(0);
                    }
                }
                else if (pid == 0)
                {
                    mostrarInfo(getpid(), 3, getppid(), "quinto bisnieto", "Zombie");

                    // daemon
                    /*pid = fork();
                    if (pid > 0)
                    {
                        exit(0);
                    }
                    else if (pid == 0)
                    {
                        mostrarInfo(getpid(), 4, 1, "", "demonio");
                        while (1)
                        {
                        }
                    }
                    */
                    switch (fork())
                    {
                    case -1:
                        return -1;
                    case 0:
                        //mostrarInfo(getpid(), 4, 1, "", "demonio");
                        break;
                    default:
                        _exit(EXIT_SUCCESS);
                    }

                    if (setsid() == -1)
                        return -1;

                    switch (fork())
                    {
                    case -1:
                        return -1;
                    case 0:
                        break;
                    default:
                        _exit(EXIT_SUCCESS);
                    }
                    mostrarInfo(getpid(), 4, 1, "", "demonio");
                }
            }
        }

        while (1)
        {
        }

        return EXIT_SUCCESS;
    }
}