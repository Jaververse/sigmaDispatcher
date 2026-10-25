#include "../include/sync.h"
#include <stdlib.h>
#include <stdio.h>

void init_sync(SyncThreads *sync_threads) { //inicializa el mutex, contadores y variables tanto pora el trabajador como el maestro
if (!sync_threads) return;
pthread_mutex_init(&sync_threads->mutex, NULL);
pthread_cond_init(&sync_threads->cond_master, NULL);
pthread_cond_init(&sync_threads->cond_worker, NULL);
sync_threads->current_tick = 0;
sync_threads->master_ready = 0;
sync_threads->worker_ready = 0;
sync_threads->simulation_finished = 0;
}

void destroy_sync(SyncThreads *sync_threads) { //Destruye y libera los recursos, si ha terminado la simulacion
if (!sync_threads) return;
pthread_mutex_destroy(&sync_threads->mutex);
pthread_cond_destroy(&sync_threads->cond_master);
pthread_cond_destroy(&sync_threads->cond_worker);
}