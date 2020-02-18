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
#include <ctype.h>
#include <thread>
#include <iostream>
#include <sstream> 
#include <vector>
#include <fstream>
#include <string>
#include <math.h>
#include <fstream>

#define MAX_DATOS 2048

#define RESET       "\033[0m"
#define RED         "\033[31m"      /* Red */
#define GREEN       "\033[32m"      /* Green */
#define YELLOW      "\033[33m"      /* Yellow */
#define BLUE        "\033[34m"      /* Blue */
#define MAGENTA     "\033[35m"      /* Magenta */
#define CYAN        "\033[36m"      /* Cyan */
#define WHITE       "\033[37m"      /* White */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"

std::vector<std::string> colores = {GREEN, YELLOW, BLUE, MAGENTA, CYAN, RED, WHITE};
std::vector<std::string> coloresNegrita = {BOLDGREEN, BOLDYELLOW, BOLDBLUE, BOLDMAGENTA, BOLDCYAN, BOLDRED, BOLDWHITE};
std::vector <std::vector<std::string>> resultados;
std::mutex semaforo;
void mostrarResultados();
void lanzarHilos(int hilos, std::vector<std::thread> &hilosvec);
void crearHilos(int hilos,int lineas, int lineasHilo,std::string fichero,std::string palabraComparacion,std::vector<std::thread> &hilosvec);
int saberLineas(std::string fichero);
void busqueda(int i,int lineasHilo, int lineasExtraUltimo, std::string fichero,std::string palabraComparacion);
/*###########################################################################################
#   Nombre del funcion: main
#   Fecha de creacion: 10/03/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -argc: Numero de argumentos.
#                     -argv:  Argumentos.
#
#   Descripcion: ejecuta los distintos metodos que resuelven el problema, comienza por la
#                lectura de las lineas de archivo, el calculo de las lineas por hilo,
#                la creacion de los hilos, su ejecucion y la impresion y almacenamiento
#                de los resultados.
#
############################################################################################*/

int main(int argc, char* argv[]){
    int lineas=0;
    int hilos=atoi(argv[3]);
    int lineasHilo=0;
    std::string fichero = "./Archivos/";
    fichero.append(argv[1]);
    std::string palabraComparacion = argv[2];
    std::vector<std::thread> hilosvec;

    lineas=saberLineas(fichero);

    lineasHilo=floor(lineas/hilos);

    std::cout << coloresNegrita.at(4) << "el numero de lineas es " << lineas << RESET << std::endl;

    crearHilos(hilos,lineas,lineasHilo, fichero, palabraComparacion, hilosvec);

    lanzarHilos(hilos, hilosvec);

    mostrarResultados();
    
    return EXIT_SUCCESS;
}
/*###########################################################################################
#   Nombre del funcion: mostrarResultados
#   Fecha de creacion: 1/04/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     ninguno.
#
#   Descripcion: Se encarga de recorrer el vector de resultados que contiene el vector
#                con los resultados de cada linea tratada.
#                A su vez almacena los datos en un archivo de log
#
############################################################################################*/
void mostrarResultados(){
    int indiceLinea=0;
    int indiceResultado=0;
    std::ofstream ficheroSalida;
    ficheroSalida.open ("log.txt");
    for (indiceLinea=0;indiceLinea<resultados.size();indiceLinea++){
        for(indiceResultado=0;indiceResultado<resultados.at(indiceLinea).size();indiceResultado++){
            char aux[2];
            sprintf(aux,"%c",resultados.at(indiceLinea).at(indiceResultado).at(6));
            int numero = atoi(aux)%colores.size();
            std::cout << colores.at(numero) << resultados.at(indiceLinea).at(indiceResultado) << RESET << std::endl;
            ficheroSalida << resultados.at(indiceLinea).at(indiceResultado)<<"\n";
        }
    }
    ficheroSalida.close();
}
/*###########################################################################################
#   Nombre del funcion: lanzarHilos
#   Fecha de creacion: 13/03/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -hilos: Numero de hilos.
#                     -hilosvec:  vector de hilos.
#
#   Descripcion: ejecuta los hilos creados.
#
############################################################################################*/
void lanzarHilos(int hilos,std::vector<std::thread> &hilosvec){
    int hilo=0;
    for(hilo=0;hilo<hilos;hilo++){
        hilosvec.at(hilo).join();
    }
}
/*###########################################################################################
#   Nombre del funcion: lcrearHilos
#   Fecha de creacion: 13/03/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -hilos: Numero de hilos.
#                     -lineas: lienas totales.
#                     -lineasHilo: Numero de lineas por hilo.
#                     -fichero:  fichero a tratar.
#                     -palabraComparacion:  palabra a comparar.
#                     -hilosvec:  vector de hilos.
#
#   Descripcion: crea los hilos..
#
############################################################################################*/
void crearHilos(int hilos,int lineas, int lineasHilo,std::string fichero, std::string palabraComparacion, std::vector<std::thread> &hilosvec){
    int hilo;
    int lineasExtraUltimo=0;
    for(hilo=0;hilo<hilos;hilo++){
        if(hilo==hilos-1){
            lineasExtraUltimo=lineas-(lineasHilo*(hilo))-lineasHilo;
        }
        std::thread mi_hilo(busqueda,hilo,lineasHilo,lineasExtraUltimo, fichero, palabraComparacion);
        hilosvec.push_back(std::move(mi_hilo));
    }
}
/*###########################################################################################
#   Nombre del funcion: saberLineas
#   Fecha de creacion: 12/03/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -fichero:  fichero a tratar.
#
#   Descripcion: recorre el fichero para saber las lineas de este.
#
############################################################################################*/
int saberLineas(std::string fichero){
    int lineas=0;
    std::ifstream archivo (fichero, std::ifstream::in);
    std::string linea;

    while(std::getline(archivo,linea,'\n')){
            lineas++;
    }

    archivo.close();

    return lineas;
}
/*###########################################################################################
#   Nombre del funcion: saberLineas
#   Fecha de creacion: 14/03/2019
#   Version: 7.2
#   Argumentos de entrada:
#                     -i:  id del hilo.
#                     -lineasHilo:  lineas de cada hilo.
#                     -lineasExtraUltimo:  lineas de mas para el ultimo hilo.
#                     -fichero:  fichero a tratar.
#                     -palabraComparacion:  palabra a comparar.
#
#   Descripcion: el hilo recorre el fichero en funcion de las lineas que le toca comprobar
#                trata esas lineas y realiza la comparacion guardando registros de ello
#
############################################################################################*/
void busqueda(int i, int lineasHilo, int lineasExtraUltimo, std::string fichero,std::string palabraComparacion){
    std::string linea;
    char char_fichero;
    std::string palabra;
    std::string palabraPosterior;
    std::string palabraAnterior;
    std::ifstream archivo (fichero, std::ifstream::in);
    int indiceLinea=0;
    int indiceLineaTratada=0;
    int inicio=(i*lineasHilo);
    int finalhilo=(i*lineasHilo)+lineasHilo+(lineasExtraUltimo);
    int numeroLinea=1;
    std::vector <std::vector<std::string>> vectorLineas;
    std::vector <std::string> resultado;
    int indiceResultadoLinea=0;

    semaforo.lock();
    std::cout << coloresNegrita.at(i%coloresNegrita.size())<< "Soy el hilo: " << i << RESET << std::endl;
    semaforo.unlock();

    while(std::getline(archivo,linea,'\n')){
        if(numeroLinea>inicio&&numeroLinea<=finalhilo){
            std::string lineatratada;
        for(indiceLinea=0;indiceLinea<linea.size();indiceLinea++){
            char_fichero=linea.at(indiceLinea);
            if(char_fichero!='.' && char_fichero!=',' && char_fichero!=':' && char_fichero!='!' && char_fichero!='?'){
                lineatratada[indiceLineaTratada++]=tolower(char_fichero);
            }
        }
        std::istringstream lineas_archivo_string(lineatratada.c_str());
        std::string token;
        std::vector <std::string> linea;
        while(std::getline(lineas_archivo_string,token,' ')){
            linea.push_back(token);
        }
        vectorLineas.push_back(linea);
        }
        indiceLineaTratada=0; 
        numeroLinea++;
    }

    for(indiceLinea=0;indiceLinea<vectorLineas.size();indiceLinea++){
        for(indiceResultadoLinea=0;indiceResultadoLinea<vectorLineas.at(indiceLinea).size();indiceResultadoLinea++){
                    if(vectorLineas.at(indiceLinea).at(indiceResultadoLinea).compare(palabraComparacion)==0){
                        try
                        {
                            palabraPosterior=vectorLineas.at(indiceLinea).at(indiceResultadoLinea+1).c_str();
                        }
                        catch(const std::out_of_range e)
                        {
                            palabraPosterior.clear();
                        }
                        try
                        {
                            palabraAnterior=vectorLineas.at(indiceLinea).at(indiceResultadoLinea-1).c_str();
                        }
                        catch(const std::out_of_range e)
                        {
                            palabraAnterior.clear();
                        }
                            palabra=vectorLineas.at(indiceLinea).at(indiceResultadoLinea).c_str();
                        std::string result;
                        result="[Hilo "+std::to_string(i)+" inicio:"+std::to_string(inicio+1)+" - final: "+std::to_string(finalhilo)+"] :: línea "+std::to_string(indiceLinea+1+inicio)+" :: …  "+palabraAnterior+" "+palabra+" "+palabraPosterior+"  …\n";
                        
                        resultado.push_back(result);
                        
                    }
        }
        semaforo.lock();
        resultados.push_back(resultado);
        resultado.clear();
        semaforo.unlock();
        
    }

    archivo.close();
}