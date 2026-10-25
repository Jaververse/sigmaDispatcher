#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"
#include "metrics.h"
#include "logger.h"
#include "scheduler.h"
#include "tcb.h"
#include "threads.h"

int main(int argc, char *argv[]) {
    char *input_file = NULL;//Ruta del escenario
    bool verbose = false;//Modo depuracion

    //Procesamiento de argumentos de consola (CLI)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_file = argv[++i];
        }
    }

    logger_init(verbose);
    //Sin archivo de entrada no hay nada que simular, salimos con error
    if (!input_file) {
        logger_log_error("Argumentos insuficientes. Uso: ./sentinel-dispatcher [-v] --input <input_file>");
        exit(EXIT_FAILURE);
    }

    //Inicializacion del sistema
    //schedulerInit reserva memoria para las 3 colas
    scheduler_t *scheduler = schedulerInit();
    if (!scheduler) {
        logger_log_error("Fallo al inicializar el planificador.");
        exit(EXIT_FAILURE);
    }
    /*Parseamos el archivo: 'tasks' sera un arreglo dinamico de punteros a tcb_t y 'num_tasks' 
    se llena por referencia con la cantidad de centinelas leidos*/
    int num_tasks = 0;
    tcb_t **tasks = parse_input_file(input_file, &num_tasks);
    if (!tasks || num_tasks == 0) {
        logger_log_error("Fallo al cargar el archivo de escenario o archivo vacio.");
        schedulerDestroy(scheduler);
        exit(EXIT_FAILURE);
    }
    //Imprime la cabecera con los parametros fijos (quantums) justo antes de simular
    logger_system_startup();



    // ZONA MULTIHILO POSIX 

    ThreadManager thread_manager;
    
    // Iniciamos el Hilo Maestro y el Hilo Trabajador (CPU_0) pasándoles el scheduler y las tareas
    if (start_thread_manager(&thread_manager, scheduler, tasks, num_tasks, verbose) != 0) {
        logger_log_error("Fallo al iniciar los hilos de concurrencia.");
        schedulerDestroy(scheduler);
        exit(EXIT_FAILURE);
    }

    // Esperamos (join) a que ambos hilos terminen la simulación de forma sincronizada tic a tic
    join_thread_manager(&thread_manager);



    // 3. Impresión de Métricas y Limpieza
    metrics_print_global(scheduler->timer, num_tasks, tasks);

    for (int i = 0; i < num_tasks; i++) {
        tcbDestroy(tasks[i]);
    }
    free(tasks);
    schedulerDestroy(scheduler);

    return 0;
}