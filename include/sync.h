#ifndef SYNC_H
#define SYNC_H

#include <pthread.h>

// Estructura para la sincronización basada en hilos (Mutex y Variables de Condición)
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond_master;
    pthread_cond_t cond_worker;
    int current_tick;
    int master_ready;
    int worker_ready;
    int simulation_finished;
} SystemSync;

// Funciones de inicialización, destrucción y control de sincronización por tic
void init_sync(SystemSync *sync_sys);
void destroy_sync(SystemSync *sync_sys);
void wait_for_master(SystemSync *sync_sys, int tick);
void signal_worker_done(SystemSync *sync_sys, int tick);
void wait_for_worker(SystemSync *sync_sys, int tick);
void signal_master_done(SystemSync *sync_sys, int tick);

#endif // SYNC_H
