#ifndef METRICS_H
#define METRICS_H

#include "tcb.h"

// Calcula e imprime las estadísticas globales de la simulacion
// Recibe el tiempo total simulado, la cantidad de tareas y el arreglo de tareas finalizadas
void metrics_print_global(int total_ticks, int num_completed_tasks, tcb_t** tasks);

#endif