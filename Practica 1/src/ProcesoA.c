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
#   Fecha de creacion: 11/02/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -argc: Numero de argumentos.
#                     -argv:  Argumentos.
#
#   Descripcion: Lee el fichero de estudiantes y obtiene los dni para crear los directorios
#                con nombre de esos dni.
#
#
############################################################################################*/

int main(int argc, char *argv[]){
    char lineas_archivo[400];
    char char_fichero;
    char *token;
    char directorio[80];
    const char separador[2] = " ";
    int indice_char=0;
    FILE *archivo;

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

    token = strtok(lineas_archivo, separador);

    while( token != NULL ) {
        sprintf(directorio,"./dirs/%s",token);
        token = strtok(NULL, separador);
        token = strtok(NULL, separador);
        token = strtok(NULL, separador);
        if(mkdir(directorio ,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
            fprintf(stderr,"Error creando directorio %s: %s\n",directorio,strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    fclose(archivo);
    return EXIT_SUCCESS;
}