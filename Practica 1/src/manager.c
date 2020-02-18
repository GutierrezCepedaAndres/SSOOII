/*###########################################################################
##                      Sistemas Operativos II                             ##
##    Autor: Andres Gutierrez Cepeda        Curso: 2018 / 2019             ##
###########################################################################*/

#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define ESCRITURA 1 
#define LECTURA 0
#define PIDA 0
#define PIDB 1
#define PIDC 2
#define PIDD 3

pid_t pids[4];

pid_t crearProceso(char nombre_proceso []);
void manejador(int signal);
void escribirLog(char cadena_log []);

/*###########################################################################################
#   Nombre del funcion: main
#   Fecha de creacion: 11/02/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -argc: Numero de argumentos.
#                     -argv:  Argumentos.
#
#   Descripcion: Comprueba el numero de argumentos pasados y ejecuta y espera
#                a los procesos.
#
#               Genera un archivo log.txt que almacena el registro de finalizacion
#               de procesos y la nota media de la clase. En caso de finalizacion
#               por el usuario se guarda en el log que se produjo una finalizacion
#               forzosa.
#
############################################################################################*/

int main( int argc, char *argv[]){
    int estado;
    signal (SIGINT, manejador);
    escribirLog("******** Log del sistema ********\n");

    if (argc!=1){
        printf("\nError se han introducido argumentos, finalizando programa.\n");
        exit(EXIT_FAILURE);
    }

    pids[PIDA]=crearProceso("ProcesoA");
    waitpid(pids[PIDA],&estado,0);
    escribirLog("Creación de directorios finalizada.\n");
    pids[PIDB]=crearProceso("ProcesoB");
    pids[PIDC]=crearProceso("ProcesoC");
    waitpid( pids[PIDB],&estado,0);
    escribirLog("Copia de modelos de examen, finalizada.\n");
    waitpid(pids[PIDC],&estado,0);
    escribirLog("Creación de archivos con nota necesaria para alcanzar la nota de corte, finalizada.\n");
    escribirLog("FIN DE PROGRAMA\n");

    return EXIT_SUCCESS;
}

/*###########################################################################################
#   Nombre del funcion: crearProceso
#   Fecha de creacion: 11/02/2019
#   Version: 2.0 modificacion el 02/03/2019
#   Argumentos de entrada:
#                     -nombre_proceso: Nombre del proceso que se va a crear y ejecutar.
#
#   Descripcion: Crea un nuevo proceso y ejecuta el codigo del mismo, en caso de ser el
#                proceso C este llevara como argumento el descriptor.
#
############################################################################################*/

pid_t crearProceso(char nombre_proceso []){
    pid_t  pid;
    char *ruta;
    char wr_tuberia[256];
    char buffer[4];
    char *parmList[] = {NULL};
    char *parmListC[] = {wr_tuberia,NULL};
    char *envParms[] = {NULL};
    char cadena_log[400];
    int tuberia[2];

    pipe(tuberia);
    sprintf(wr_tuberia, "%d", tuberia[ESCRITURA]);
    ruta = malloc(8*sizeof(char));
    sprintf(ruta,"./exec/%s",nombre_proceso);
    
    switch(pid=fork()){

            case -1:
            printf("\nNo se ha podido crear el %s\n",nombre_proceso);
            exit(EXIT_FAILURE);
            
            case 0:
            printf("\nEl %s se ha creado correctamente\n",nombre_proceso);
                if(strcmp(nombre_proceso,"ProcesoC")!=0){
                    if(execve(ruta,parmList, envParms)==-1){
                        fprintf(stderr,"\nSe ha producido un error en la ejecucion del %s: %s\n",nombre_proceso,strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                }else{
                    if(execve(ruta,parmListC, envParms)==-1){
                        fprintf(stderr,"\nSe ha producido un error en la ejecucion del proceso: %s\n",strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                }
            break;

            default:
                if(strcmp(nombre_proceso,"ProcesoC")==0){
                    read(tuberia[LECTURA], buffer, sizeof(buffer));
                    sprintf(cadena_log,"La nota media de la clase es: %s\n",buffer);
                    escribirLog(cadena_log);
                }else{
                    break;
                }
    }

    return pid;
}

/*###########################################################################################
#   Nombre del funcion: manejador
#   Fecha de creacion: 02/03/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -signal: señal que debe de ejecutar.
#
#   Descripcion: mata los procesos y ejecuta el proceso D, espera a que este finalice
#                y registra la finalizacion por interrupcion de usuario.
#
############################################################################################*/

void manejador(int signal){
    int estado;

    kill(pids[PIDA], SIGKILL);
    kill(pids[PIDB], SIGKILL);
    kill(pids[PIDC], SIGKILL);
    pids[PIDD]=crearProceso("ProcesoD");
    waitpid(pids[PIDD],&estado,0);
    escribirLog("FIN DE PROGRAMA INTERRUPCION POR EL USUARIO\n");

    exit(EXIT_FAILURE);
}

/*###########################################################################################
#   Nombre del funcion: escribirLog
#   Fecha de creacion: 01/03/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -cadena_log: cadena que debe escribirse en el log.
#
#   Descripcion: escribe en una nueva linea del fichero log los nuevos datos de ejecucion.
#
############################################################################################*/

void escribirLog(char cadena_log []){
    FILE *archivo;

    archivo=fopen("log.txt","a");
    if(fwrite(cadena_log, 1, (strlen(cadena_log))*sizeof(char),archivo) == -1){
        fprintf(stderr,"\nError al escribir en el archivo log.txt: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    fclose(archivo);
}