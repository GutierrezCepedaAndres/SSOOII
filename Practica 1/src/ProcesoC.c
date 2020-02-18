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

void mandarMedia(int nota_media, int argv_0, int alumnos);

/*###########################################################################################
#   Nombre del funcion: main
#   Fecha de creacion: 17/02/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -argc: Numero de argumentos.
#                     -argv:  Argumentos.
#
#   Descripcion: Calcula la nota que tiene que obtener el estudiante para que le de lña media.
#                A su vez tambien manda la nota media de los estudiantes al proceso manager.
#
############################################################################################*/

int main(int argc, char *argv[]){
    char lineas_archivo[400];
    char char_fichero;
    char dni[80];
    char cadena[200];
    char directorio[80];
    char *token;
    const char separador[2] = " ";
    FILE *archivo;
    int indice_char=0;
    int nota_obtenida;
    int nota_a_obtener;
    int nota_media=0;
    int alumnos=0;

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
        sprintf(dni,"%s",token);
        token = strtok(NULL, separador);
        token = strtok(NULL, separador);
        nota_obtenida=atoi(token);
        nota_media+=nota_obtenida;
        nota_a_obtener=((10-nota_obtenida));
        token = strtok(NULL, separador);
        alumnos++;
        sprintf(cadena,"La nota que debes obtener en este nuevo examen para superar la prueba es %d",nota_a_obtener);
        sprintf(directorio,"./dirs/%s/nota.txt",dni);
        archivo=fopen(directorio,"wb");  ;
        
            if(fwrite(cadena, 1, (strlen(cadena))*sizeof(char),archivo) == -1){
                fprintf(stderr,"Error al escribir el archivo nota.txt: %s\n",strerror(errno));
                exit(EXIT_FAILURE);
            }
        fclose(archivo);  
    }
    
    mandarMedia(nota_media, atoi(argv[0]), alumnos);

    return EXIT_SUCCESS;
}

/*###########################################################################################
#   Nombre del funcion: mandarMedia
#   Fecha de creacion: 17/02/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -nota_media: suma de toda las notas de los estudiantes para calcular media.
#                     -argv_0:  valor del primer argumento pasado por la linea de argumentos.
#                     -alumnos: cantidad de alumnos que hay en la clase.
#
#   Descripcion: Calcula la media y se la envia por medio de una tuberia al manager.
#
############################################################################################*/

void mandarMedia(int nota_media, int argv_0, int alumnos){
    char nota_media_cadena[100];
    nota_media=nota_media/(alumnos);
    sprintf(nota_media_cadena,"%d",nota_media);
    write(argv_0, nota_media_cadena, strlen(nota_media_cadena)+1);
}



