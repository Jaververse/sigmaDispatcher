#include <stdio.h>
#include "../include/logger.h"
#include "../include/queue.h"

static bool is_verbose = false;

void logger_init(bool verbose_mode) {
    is_verbose = verbose_mode;
}

void logger_system_startup() {

    printf("Fixed parameters: Queue 0 [Q=%d] | Queue 1 [Q=%d] | Queue 2 [FCFS]\n\n", QUANTUM_0, QUANTUM_1);
    
    if (is_verbose) {
        printf("[DEBUG] Iniciando orquestador en Modo Depuracion (Verbose)...\n\n");
    }
}

void logger_log_error(const char* msg) {
    fprintf(stderr, "[ERROR] %s\n", msg);
}