#ifndef PARSER_H
#define PARSER_H

#include "tcb.h"

// Lee el archivo de escenario. 
// Retorna un arreglo de punteros a tcb_t.
// 'num_tasks' se actualizara con la cantidad de centinelas leidos.
tcb_t** parse_input_file(const char *filepath, int *num_tasks);

#endif