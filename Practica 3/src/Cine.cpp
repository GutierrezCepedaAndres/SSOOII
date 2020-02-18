/*###########################################################################
##                      Sistemas Operativos II                             ##
##    Autor: Andres Gutierrez Cepeda        Curso: 2018 / 2019             ##
###########################################################################*/

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <chrono>
#include <condition_variable>
#include <future>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

#define RESET       "\033[0m"
#define RED         "\033[31m"      /* Red */
#define GREEN       "\033[32m"      /* Green */
#define YELLOW      "\033[33m"      /* Yellow */
#define BLUE        "\033[34m"      /* Blue */
#define MAGENTA     "\033[35m"      /* Magenta */
#define WHITE       "\033[37m"      /* White */
#define CYAN        "\033[36m"      /* Cyan */

int MAX_PETICION_ENTRADAS = 7;
int MAX_PETICION_COMIDA = 5;
int MAX_TURNO_PAGO_TAQUILLA=100;
int MAX_TURNO_PAGO_COMIDA=40;
int MAX_HORIZONTAL = 12;
int MAX_VERTICAL = 6;
int PRECIO_TICKET = 7;

struct MsgPeticionTicket{
    int id_cliente;
    int n_tickets;
    int turno_pago;
    bool b_atendido;

    MsgPeticionTicket (int id, int nt) :id_cliente(id), n_tickets(nt){
        turno_pago=0;
        b_atendido = false;
    }

};

struct MsgPeticionComida{
    int id_cliente;
    int n_palomitas;
    int n_bebida;
    int turno_pago;
    bool b_atendido;

    MsgPeticionComida (int id, int np, int nb) :id_cliente(id), n_palomitas(np), n_bebida(nb){
        turno_pago=0;
        b_atendido = false;
    }

};

struct MsgPeticionReponedor{
    int id_puesto;
    int palomitas;
    int bebidas;
    bool b_atendido;
    MsgPeticionReponedor (int id, int n_palomitas, int n_bebidas) :id_puesto(id), palomitas(n_palomitas), bebidas(n_bebidas){
        b_atendido = false;
    }
};

std::mutex s_taquilla, s_manager, s_solicitud_taquilla, s_espera_tickets, s_pago, s_espera_pago;
std::mutex s_clientes_comida, s_mutex_comida, s_mutex_pago;
std::mutex s_fin_comida;
std::mutex s_reponedor;
std::mutex s_sistema_pago;
std::mutex s_fin_reponedor;
std::mutex s_imprimir;

std::condition_variable cv_tickets;
std::condition_variable cv_clientes_comida;
std::condition_variable cv_comida_atendida;
std::condition_variable cv_reponedor;
std::condition_variable cv_sistema_pago;
std::condition_variable cv_puesto_atendido;

std::vector<std::thread> v_puestos_comida;
std::queue<std::thread> cola_tickets;
std::queue<std::thread> cola_comida;
std::queue<std::thread> cola_atendidos;

std::priority_queue<int> cola_sistema_pago;

std::queue<MsgPeticionTicket*> cola_peticiones_tickets;
std::queue<MsgPeticionComida*>cola_peticiones_comida;
std::queue<MsgPeticionReponedor*>cola_peticiones_reponedor;

int matriz_sala [6][12];


int turno = -1;
int ganancia_tickets = 0;
int total_clientes = 0;

/*###########################################################################################
#   Nombre del funcion: escribirLog
#   Fecha de creacion: 06/05/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -std::string cadena_log: Cadenaa que debe ser escrita en el archivo log.
#
#   Descripcion: Crea una variable de tipo ofstream para abrir el archivo en formato de
#   entrada y añadir datos, se añade la cadena y se cierra el archivo.
#
############################################################################################*/

void escribirLog(std::string cadena_log){

    std::ofstream archivo;
    archivo.open("dirs/log.txt",std::ofstream::out | std::ofstream::app);
    archivo << cadena_log;
    archivo.close();
}

/*###########################################################################################
#   Nombre del funcion: imprimirMensaje
#   Fecha de creacion: 29/04/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -std::string mensaje: Cadenaa que debe ser mostrada por pantalla.
#                     -std::string color: Color con el que se debe imprimir el mensaje.
#
#   Descripcion: Bloqueamos el semaforo para la impresion de mensajes y mostramos por pantalla
#   el mensaje en el color indicado, tambien llamamos al metodo que escribira el mensaje en el
#   log y desbloqueamos el semaforo para que otro mensaje pueda ser impreso.
#
############################################################################################*/

void imprimirMensaje(std::string mensaje, std::string color){
    s_imprimir.lock();
    std::cout << color << mensaje << RESET;
    escribirLog(mensaje);
    s_imprimir.unlock();
}

/*###########################################################################################
#   Nombre del funcion: manejador
#   Fecha de creacion: 06/05/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -int signal: señan que controlara el manejador.
#
#   Descripcion: Al finalizar el programa lo primero que haremos sera mostrar la ganancia de
#   la sesion de hoy, junto con la cantidad de clientes atendidos, tambien se mostrara la
#   distribucion de los clientes en la sala, tras esto se escribira en el log la finalizacion
#   del trabajo y se finaliza el programa.
#
############################################################################################*/

void manejador(int signal){
    
    int fila = 0;
    int columna  = 0;
    std::string mensaje;

    imprimirMensaje("MANEJADOR: La ganancia del dia de hoy es de: " + std::to_string(ganancia_tickets) + " con un total de clientes de: " + std::to_string(total_clientes) + "\n",CYAN);
    imprimirMensaje("MANEJADOR: Valores de los asientos:\n",CYAN);

    for(fila=0;fila<MAX_VERTICAL;fila++){
        imprimirMensaje("\n",CYAN);
        for(columna=0;columna<MAX_HORIZONTAL;columna++){
            std::cout << CYAN << std::setfill('0') << std::setw(2) << matriz_sala[fila][columna] << " " << RESET;
            escribirLog(std::to_string(matriz_sala[fila][columna]) + " ");
        }
    }

    escribirLog("FIN DE PROGRAMA INTERRUPCION POR EL USUARIO\n");

    exit(EXIT_SUCCESS);
}

/*###########################################################################################
#   Nombre del funcion: comprobarSala
#   Fecha de creacion: 10/04/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -int n_tickets: numero de tickets que solicita el cliente.
#                     -int id_cliente: id del cliente para asignarle un asiento.
#
#   Descripcion: Se recorre la matriz y se comprueba que caben todos los clientes solicitados
#   en la sala, de poderse añadir, se iran añadiendo de uno en uno.
#
############################################################################################*/

bool comprobarSala(int n_tickets, int id_cliente){
    int fila = 0;
    int columna  = 0;
    int ocupadas = 0;
    bool asignado = false;

    for(fila=0;fila<MAX_VERTICAL;fila++){
        for(columna=0;columna<MAX_HORIZONTAL;columna++){
            //no va bien, esta fallando en esta parte
            if(matriz_sala[fila][columna]==0 && (MAX_HORIZONTAL*MAX_VERTICAL)-ocupadas>=n_tickets-- && n_tickets>=0){
                matriz_sala[fila][columna]=id_cliente;
                asignado = true;
            }
            ocupadas++;
        }
    }
    return asignado;
}

/*###########################################################################################
#   Nombre del funcion: generaNumeroAleatorio
#   Fecha de creacion: 10/04/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -int limite: Limite superior hasta el que se podran generar numeros
#                     aleatorios.
#
#   Descripcion: Genera un numero aleatorio enre 0 y el limite.
#
############################################################################################*/

int generaNumeroAleatorio(int limite){
    return (rand()%(limite-1))+1;
}

/*###########################################################################################
#   Nombre del funcion: cliente
#   Fecha de creacion: 10/04/2019
#   Version: 3.0
#   Argumentos de entrada:
#                     -int id_cliente: identificador del cliente.
#
#   Descripcion: Lo primero que hace es esperar su turno a ser atendido en taquilla,
#   una vez accede a la taquilla crea su mensaje de peticion de entradas y lo añade a
#   la cola de peticiones de tickets, si ha sido atendido en la taquilla crea el 
#   mensaje de peticion de comida y es traansferido a la cola de peticiones de comida
#   cuando es atendido en la cola de comida el cliente finaliza su ejecucion.
#
############################################################################################*/

void cliente (int id_cliente){
    imprimirMensaje("Cliente " + std::to_string(id_cliente) + " creado y esperando en cola para tickets ...\n",GREEN);

    std::unique_lock<std::mutex> lk (s_taquilla);
    cv_tickets.wait(lk, [id_cliente]{return (turno == id_cliente);});
    imprimirMensaje("CLIENTE " + std::to_string(id_cliente) + ": es mi turno para la taquilla!\n",GREEN);
    lk.unlock();

    //genera peticion
    MsgPeticionTicket mpt(id_cliente,generaNumeroAleatorio(MAX_PETICION_ENTRADAS));
    cola_peticiones_tickets.push(&mpt);

    s_solicitud_taquilla.unlock();
    s_espera_tickets.lock();

    bool *p_flag_atendido = &(mpt.b_atendido);
    if(*p_flag_atendido==true){
    imprimirMensaje("CLIENTE " + std::to_string(id_cliente) + ": atendiendo en TAQUILLA, a la espera en cola de comida!\n",GREEN);
    s_manager.unlock();


    //crea peticion de comida:
    MsgPeticionComida mpc(id_cliente, generaNumeroAleatorio(MAX_PETICION_COMIDA),generaNumeroAleatorio(MAX_PETICION_COMIDA));
    cola_peticiones_comida.push(&mpc);

    //notificacion a un puesto de venta de comida y bebida.
    cv_clientes_comida.notify_one();

    //espera a confirmacion de punto de venta
    std::unique_lock<std::mutex> lk_comida (s_fin_comida);
    bool *p_flag_atendido = &(mpc.b_atendido);
    cv_comida_atendida.wait(lk_comida, [p_flag_atendido]{return *p_flag_atendido;});
    
    imprimirMensaje("CLIENTE " + std::to_string(id_cliente) + " FINALIZA\n",GREEN);
    }else{
        s_manager.unlock();
        imprimirMensaje("CLIENTE " + std::to_string(id_cliente) + " FINALIZA\n",GREEN);
    }
}

/*###########################################################################################
#   Nombre del funcion: taquilla
#   Fecha de creacion: 10/04/2019
#   Version: 2.0
#   Argumentos de entrada: ninguno.
#
#   Descripcion: La taquilla ira sacando personas de la cola de peticion de tickets
#   comprobaara si la sala dispone de asientos para atender al cliente y de disponer de
#   ellos generara un turno para ser acceder al sistema de pago, cuando sea su turno,
#   la taquilla accedera al sistema de pago y pasara al cliente a la cola de peticiones
#   de comida.
#
############################################################################################*/

void taquilla(){
    //Damos tiempo a B para su bloqueo.
    imprimirMensaje("Taquilla ABIERTA\n",BLUE);
    while(true){
        s_solicitud_taquilla.lock();

        MsgPeticionTicket *mpt = cola_peticiones_tickets. front();
        cola_peticiones_tickets.pop();

        imprimirMensaje("TAQUILLA: venta de " + std::to_string(mpt->n_tickets) + " tickets a cliente " + std::to_string(mpt->id_cliente) + "...\n",BLUE);
        if(comprobarSala(mpt->n_tickets,mpt->id_cliente)){
            //std::cout << BLUE <<"TAQUILLA: Solicitando sistema de pago\n" << RESET << std::endl;
            std::this_thread::sleep_for (std::chrono::seconds(1));
            total_clientes+=mpt->n_tickets;
            //solicitud de pago en exclusion mutua
            mpt->turno_pago=generaNumeroAleatorio(MAX_TURNO_PAGO_TAQUILLA);
            cola_sistema_pago.push(mpt->turno_pago);
            std::unique_lock<std::mutex> lk2 (s_sistema_pago);
            cv_sistema_pago.wait(lk2, [mpt]{return (cola_sistema_pago.top() == mpt->turno_pago);});
            cola_sistema_pago.pop();
            lk2.unlock();
            s_mutex_pago.lock();
            imprimirMensaje("TAQUILLA: Solicitando sistema de pago\n",BLUE);
            s_pago.unlock();//despertamos servicio de pago
            s_espera_pago.lock();//esperamos a confirmacion de pago
            ganancia_tickets+=mpt->n_tickets*PRECIO_TICKET;
            s_mutex_pago.unlock();//fin seccion critica.
            lk2.lock();
            mpt->b_atendido=true;
            //cliente atendido y cambio a cola de comida
            cola_comida.push(move(cola_tickets.front()));
            cola_tickets.pop();
        }else{
                imprimirMensaje("TAQUILLA: No quedan suficientes entradas para este cliente.\n",BLUE);
                mpt->b_atendido=false;
        }
            //desbloquea a cliente que espera por tickets.
            s_espera_tickets.unlock();
    }
}

/*###########################################################################################
#   Nombre del funcion: sistemaPago
#   Fecha de creacion: 10/04/2019
#   Version: 2.0
#   Argumentos de entrada: ninguno.
#
#   Descripcion: El sistema de pago espera la peticion de pago.
#
############################################################################################*/

void sistemaPago(){
    imprimirMensaje("SISTEMA DE PAGO CREADO ...\n",YELLOW);

    while(true){
        s_pago.lock();
        imprimirMensaje("SP: Recibida solicitud de pago ...\n",YELLOW);
        std::this_thread::sleep_for (std::chrono::milliseconds(500));
        imprimirMensaje("SP: PAGO REALIZADO ...\n",YELLOW);
        s_espera_pago.unlock();
    }
}

/*###########################################################################################
#   Nombre del funcion: puestoComida
#   Fecha de creacion: 10/04/2019
#   Version: 5.0
#   Argumentos de entrada:
#                     -int id_puesto: identificador del puesto.
#                     -int palomitas: cantidad de palomitas con las que comienza el puesto.
#                     -int bebidas: cantidad de bebidas con las que comienza el puesto.
#
#   Descripcion: El puesto de comida al igual que la taquilla comenzara a atender
#   peticiones de la cola de peticiones de comida, accedera al sistema de pago en funcion
#   de su turno generado y llamara a un reponedor en caso de no contar con palomitas
#   suficientes para atender al cliente.
#
############################################################################################*/

void puestoComida(int id_puesto, int palomitas, int bebidas){
    imprimirMensaje("Puesto de comida " +  std::to_string(id_puesto) + " creado, esperando por clientes ...\n",MAGENTA);

    std::unique_lock<std::mutex>  lk (s_clientes_comida);

    while(true){
        
        cv_clientes_comida.wait(lk, []{return !cola_comida.empty();});
        
        s_mutex_comida.lock(); //acceso a cola de peticiones de comida en exclusion mutua
        //se atiende peticion de comida
        MsgPeticionComida *mpc = cola_peticiones_comida.front();
        cola_peticiones_comida.pop();
        std::thread cliente=move(cola_comida.front());
        cola_comida.pop();
        s_mutex_comida.unlock();

        lk.unlock();
        imprimirMensaje("PUESTO DE VENTA " + std::to_string(id_puesto) + " atiende a cliente " + std::to_string(mpc->id_cliente) + " [Palomitas:" + std::to_string(mpc->n_palomitas) + ", Bebida:" + std::to_string(mpc->n_bebida) + "]\n",MAGENTA);
        if((bebidas-mpc->n_bebida)<0 || (palomitas-mpc->n_palomitas)<0){
            MsgPeticionReponedor mpr(id_puesto,bebidas,palomitas);
            cola_peticiones_reponedor.push(&mpr);
            imprimirMensaje("PUESTO DE VENTA: " + std::to_string(id_puesto) + "Llama al reponedor\n",MAGENTA);
            //std::lock_guard<std::mutex> lk (s_reponedor);
            cv_reponedor.notify_one();
            std::unique_lock<std::mutex> lk_reponedor (s_fin_reponedor);
            bool *p_flag_atendido = &(mpr.b_atendido);
            cv_puesto_atendido.wait(lk_reponedor, [p_flag_atendido]{return *p_flag_atendido;});
            palomitas=mpr.palomitas;
            bebidas=mpr.bebidas;
        }
        //std::cout << MAGENTA <<"PUESTO DE VENTA: "<< id_puesto << "tiene una cantidad de palomitas y de bebidas de: \n"<< palomitas << " " << bebidas << RESET << std::endl;
        palomitas-=mpc->n_palomitas;
        bebidas-=mpc->n_bebida;
        //simulamos tiempo de atencion de servicio
        std::this_thread::sleep_for(std::chrono::seconds(2));

        //se realiza el pago
        //std::cout << MAGENTA <<"PUESTO DE VENTA "<< id_puesto << " solicita pago...\n" << RESET << std::endl;
        mpc->turno_pago=generaNumeroAleatorio(MAX_TURNO_PAGO_COMIDA);
        cola_sistema_pago.push(mpc->turno_pago);
        std::unique_lock<std::mutex> lk2 (s_sistema_pago);
        cv_sistema_pago.wait(lk2, [mpc]{return (cola_sistema_pago.top() == mpc->turno_pago);});
        cola_sistema_pago.pop();
        lk2.unlock();
        s_mutex_pago.lock();
        imprimirMensaje("PUESTO DE VENTA " + std::to_string(id_puesto) + " solicita pago...\n",MAGENTA);
        s_pago.unlock();
        s_espera_pago.lock();
        s_mutex_pago.unlock();
        lk2.lock();

        //manda notificacion a cliente
        imprimirMensaje("PUESTO DE VENTA " + std::to_string(id_puesto) + " Termina de atender al cliente " + std::to_string(mpc->id_cliente) + "\n",MAGENTA);

        cola_atendidos.push(move(cliente));
        mpc->b_atendido = true;
        cv_comida_atendida.notify_all();

        lk.lock();//se prepara semaforo para nuevo posible bloqueo con wait en siguiente iteracion
    }
}

/*###########################################################################################
#   Nombre del funcion: reponedor
#   Fecha de creacion: 27/04/2019
#   Version: 2.0
#   Argumentos de entrada: ninguno.
#
#   Descripcion: Atendera las peticiones de su cola de reponedor, y mandara un mensaje al
#   puesto de comida con las palomitas y bebidas que se le han suministrado en esta ocasion.
#
############################################################################################*/

void reponedor(){
    while (true){
        std::unique_lock<std::mutex> lk (s_reponedor);
        cv_reponedor.wait(lk, []{return !cola_peticiones_reponedor.empty();});
        MsgPeticionReponedor *mpr = cola_peticiones_reponedor. front();
        cola_peticiones_reponedor.pop();
        mpr->palomitas=generaNumeroAleatorio(15);
        mpr->bebidas=generaNumeroAleatorio(15);
        imprimirMensaje("REPONEDOR: Reponiendo bebidas y palomitas del puesto: " + std::to_string(mpr->id_puesto) + "\n",RED);
        std::this_thread::sleep_for(std:: chrono::milliseconds(mpr->palomitas*mpr->bebidas*7));
        mpr->b_atendido=true;
        cv_puesto_atendido.notify_all();
        lk.unlock();
    }
}

/*###########################################################################################
#   Nombre del funcion: crearClientes
#   Fecha de creacion: 02/05/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -int NUM_CLIENTES: numero de clientes que se van a crear.
#
#   Descripcion: Crear clientes y los va insertando en la cola de petticion de tickets.
#
############################################################################################*/

void crearClientes(int NUM_CLIENTES){
        for (int i=1;i<=NUM_CLIENTES;i++){
        cola_tickets.push(std::thread(cliente, i));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

/*###########################################################################################
#   Nombre del funcion: comprobarTurno
#   Fecha de creacion: 02/05/2019
#   Version: 1.0
#   Argumentos de entrada:
#                     -int NUM_CLIENTES: numero de clientes a los que asignar turno.
#
#   Descripcion: Se crea un turno y si el turno coincide con el id del cliente podra ser
#   atendido en la taquilla.
#
############################################################################################*/

void comprobarTurno(int NUM_CLIENTES){
    std::unique_lock<std::mutex> lk (s_taquilla);

    for (int i=1;i<=NUM_CLIENTES;i++){
        if(i>1){lk.lock();}
        imprimirMensaje("MANAGER: El turno es del hilo:" + std::to_string(i) + "\n",WHITE);
        turno=i;
        cv_tickets.notify_all();
        lk.unlock();
        s_manager.lock();
    }
}

/*###########################################################################################
#   Nombre del funcion: main
#   Fecha de creacion: 10/04/2019
#   Version: 7.0
#   Argumentos de entrada: ninguno.
#
#   Descripcion: inicializara algunos semaforos, algunas variables y creara y ejecutara los
#   hilos, tambien creara las taquillas.
#
############################################################################################*/

int main(){
    //metodo main - manager -

    signal (SIGINT, manejador);

    int NUM_CLIENTES = 50;
    int PUESTOS_COMIDA = 3;
    s_manager.lock();
    s_solicitud_taquilla.lock();
    s_espera_tickets.lock();
    s_pago.lock();
    s_espera_pago.lock();

    imprimirMensaje("MANAGER: Comienza la ejecucion del Programa\n",WHITE);

    //creamos una taquilla
    std::thread h_taquilla(taquilla);

    //creamos sistema de pago
    std::thread h_spago(sistemaPago);

    //creamos los puntos de venta de comida
    for(int i=1; i<= PUESTOS_COMIDA; i++){
        v_puestos_comida.push_back(std::thread(puestoComida,i,generaNumeroAleatorio(15),generaNumeroAleatorio(15)));
        std::this_thread::sleep_for(std:: chrono::milliseconds(200));
    }

    //creamos reponedor
    std::thread h_reponedor(reponedor);

    //creamos los clientes en un hilo
    std::thread h_crearClientes(crearClientes,NUM_CLIENTES);


    //generacion de turnos:
    std::thread h_turnos(comprobarTurno,NUM_CLIENTES);

    //espera a la finalizacion de hilos

    h_taquilla.join();

    return EXIT_SUCCESS;
}