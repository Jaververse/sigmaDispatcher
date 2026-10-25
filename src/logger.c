#include <stdio.h>
#include "../include/logger.h"
#include "../include/queue.h"//Para leer QUANTUM_0 y QUANTUM_1 y no escribirlos a mano

//recuerda si estamos en modo verbose desde logger_init()
static bool is_verbose = false;

void logger_init(bool verbose_mode) {
    is_verbose = verbose_mode;
}

void logger_system_startup() {
    //Los quantums salen de queue.h, asi si el equipo cambia QUANTUM_0/1 este encabezado se actualiza solo
    printf("Fixed parameters: Queue 0 [Q=%d] | Queue 1 [Q=%d] | Queue 2 [FCFS]\n\n", QUANTUM_0, QUANTUM_1);
    
    //Mensaje extra solo si el usuario pidio -v
    if (is_verbose) {
        printf("[DEBUG] Iniciando orquestador en Modo Depuracion (Verbose)...\n\n");
    }
}

void logger_log_error(const char* msg) {
     //Los errores van a stderr (no a stdout) para que no se mezclen con la salida
    //normal si alguien redirige el resultado a un archivo (./programa > salida.txt)
    fprintf(stderr, "[ERROR] %s\n", msg);
}