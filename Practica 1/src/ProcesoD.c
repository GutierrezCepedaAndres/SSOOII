/*###########################################################################
##                      Sistemas Operativos II                             ##
##    Autor: Andres Gutierrez Cepeda        Curso: 2018 / 2019             ##
###########################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>

/*###########################################################################################
#   Nombre del funcion: main
#   Fecha de creacion: 17/02/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -argc: Numero de argumentos.
#                     -argv:  Argumentos.
#
#   Descripcion: Obtiene el valor de los dni de los estudiantes y borra sus directorios.
#
############################################################################################*/

int main(int argc, char *argv[]){

    char comando[200];
    char lineas_archivo[400];
    char char_fichero;
    char *token;
    char dni[200];
    const char serparador[2] = " ";
    FILE *archivo;
    int indice_char=0;

    if((archivo=fopen("./estudiantes_p1.text","r"))==NULL){
        fprintf(stderr,"Error al abrir el fichero estudiantes_p1.text: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    while((char_fichero=fgetc(archivo))!=EOF){
        if(char_fichero!='\n'){
            lineas_archivo[indice_char]=char_fichero;
        }else{
            lineas_archivo[indice_char]=' ';
        }
        indice_char++;
    }

    token = strtok(lineas_archivo, serparador);

    while( token != NULL ) {
        sprintf(dni,"dirs/%s",token);
        token = strtok(NULL, serparador);
        token = strtok(NULL, serparador);
        token = strtok(NULL, serparador);
        sprintf(comando,"rm -r %s",dni);
        if(system((comando)) == -1){
         fprintf(stderr,"Error eliminando directorio %s: %s\n",dni,strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    fclose(archivo);

    return EXIT_SUCCESS;
}