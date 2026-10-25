#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

// Inicializa el sistema de logs y guarda el modo de ejecucion
void logger_init(bool verbose_mode);

// Imprime el encabezado inicial del simulador
void logger_system_startup();

// Imprime mensajes de error estandarizados
void logger_log_error(const char* msg);

#endif