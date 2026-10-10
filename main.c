#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"
#include "metrics.h"
#include "logger.h"
#include "scheduler.h"
#include "tcb.h"

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    bool verbose = false;

    // 1. Procesamiento de argumentos de consola (CLI)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_file = argv[++i];
        }
    }

    logger_init(verbose);

    if (!input_file) {
        logger_log_error("Argumentos insuficientes. Uso: ./sentinel-dispatcher [-v] --input <input_file>");
        exit(EXIT_FAILURE);
    }

    // 2. Inicialización del sistema
    scheduler_t *scheduler = schedulerInit();
    if (!scheduler) {
        logger_log_error("Fallo al inicializar el planificador.");
        exit(EXIT_FAILURE);
    }

    int num_tasks = 0;
    tcb_t **tasks = parse_input_file(input_file, &num_tasks);
    if (!tasks || num_tasks == 0) {
        logger_log_error("Fallo al cargar el archivo de escenario o archivo vacio.");
        schedulerDestroy(scheduler);
        exit(EXIT_FAILURE);
    }

    logger_system_startup();


    /* ZONA DE SIMULACION SECUENCIAL (Provisional para probar E/S y Métricas)
    El Integrante 2 debe reemplazar este bucle por la lógica multihilo POSIX (Master/Worker) requerida en el proyecto.*/   
    int completed_tasks = 0;
    const char* typeNames[] = {"Command", "Driller", "Heavy"};

    while (completed_tasks < num_tasks) {
        
        // Admisión
        for (int i = 0; i < num_tasks; i++) {
            if (tasks[i]->execState == NEW && tasks[i]->arrivalTime == scheduler->timer) {
                schedulerAdmit(scheduler, tasks[i]);
                printf("[T-%04d] ARRIVAL: Sentinel ID: %d [Type: %s]\n", 
                       scheduler->timer, tasks[i]->id, typeNames[tasks[i]->priority]);
            }
        }

        // Finalizacion
        if (scheduler->lastEnd != NULL) {
            printf("[T-%04d] COMPLETED: Sentinel ID: %d -> Turnaround Time: %d ticks\n", 
                   scheduler->lastEnd->returnTime, scheduler->lastEnd->id, 
                   scheduler->lastEnd->returnTime - scheduler->lastEnd->arrivalTime);
            completed_tasks++;
            scheduler->lastEnd = NULL;
        }

        if (completed_tasks >= num_tasks) break;

        // Planificacion y ejecucion
        schedulerMain(scheduler); 

        if (verbose) {
            printVerbose(scheduler);
        } else {
            printNormal(scheduler);
        }

        schedulerExecTick(scheduler);
        scheduler->timer++;
    }
    // ==============================================================================

    // 3. Impresión de Métricas y Limpieza
    metrics_print_global(scheduler->timer, num_tasks, tasks);

    for (int i = 0; i < num_tasks; i++) {
        tcbDestroy(tasks[i]);
    }
    free(tasks);
    schedulerDestroy(scheduler);

    return 0;
}