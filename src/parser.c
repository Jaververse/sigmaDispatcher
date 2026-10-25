#include <stdio.h>
#include <stdlib.h>
#include "../include/parser.h"

tcb_t** parse_input_file(const char *filepath, int *num_tasks) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        return NULL;
    }

    int capacity = 10;
    tcb_t **tasks = (tcb_t**) malloc(capacity * sizeof(tcb_t*));
    if(!tasks){
        fclose(file);
        return NULL;
    }
    
    *num_tasks = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignorar comentarios y líneas vacias
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        // Redimensionar si se supera la capacidad inicial
        if (*num_tasks >= capacity) {
            capacity *= 2;
            tcb_t **temp = realloc(tasks, capacity * sizeof(tcb_t*));
            if(!temp){
                // Error de memoria
                break;
            }
            tasks = temp;
        }

        int arrivalTime, id, priority, totalBurst;
        
        // Formato esperado: TiempoLlegada ID TipoCola Rafaga
        if (sscanf(line, "%d %d %d %d", &arrivalTime, &id, &priority, &totalBurst) == 4) {
            // Utilizamos la función del Integrante 1 para crear el bloque
            tasks[*num_tasks] = tcbInit(id, arrivalTime, priority, totalBurst);
            if(tasks[*num_tasks] != NULL){
                (*num_tasks)++;
            }
        }
    }

    fclose(file);
    return tasks;
}