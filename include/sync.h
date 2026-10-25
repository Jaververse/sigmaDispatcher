#ifndef SYNC_H
#define SYNC_H

#include <pthread.h>

//Estructura para la sincronización basada en hilos, aqui controlamos la concurrencia
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond_master; //para bloquear o desbloquear el maestro
    pthread_cond_t cond_worker; //para bloquear o desbloquear al trabajador
    int current_tick; // el contador del tic glbal
    int master_ready; // indica si el maestro termino en el tic actual
    int worker_ready; // indica si el trabajador termino en el tic actual
    int simulation_finished;
} SyncThreads;

//Funciones de inicializacion, destruccion
void init_sync(SyncThreads *sync_threads);
void destroy_sync(SyncThreads *sync_threads);

#endif
