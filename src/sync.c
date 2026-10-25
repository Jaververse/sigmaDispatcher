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

// El trabajador espera a que el maestro termine de admitir las tareas del tic actual, si el maestro no ha terminado, el trabajador se duerme
void wait_for_master(SyncThreads *sync_threads, int tick) {
pthread_mutex_lock(&sync_threads->mutex);
while (sync_threads->current_tick == tick && !sync_threads->master_ready && !sync_threads->simulation_finished) {
    pthread_cond_wait(&sync_threads->cond_worker, &sync_threads->mutex);
}
pthread_mutex_unlock(&sync_threads->mutex);
}

// El trabajador avisa que ha completado su procesamiento para el tic actual
void signal_worker_done(SyncThreads *sync_threads, int tick) {
pthread_mutex_lock(&sync_threads->mutex);
sync_threads->worker_ready = 1;
pthread_cond_signal(&sync_threads->cond_master);
pthread_mutex_unlock(&sync_threads->mutex);
}

// El maestro espera a que el trabajador procese el tic actual antes de avanzar al siguiente, si el trabajador no ha terminado, el maestro se duerme
void wait_for_worker(SyncThreads *sync_threads, int tick) {
pthread_mutex_lock(&sync_threads->mutex);
while (sync_threads->current_tick == tick && !sync_threads->worker_ready && !sync_threads->simulation_finished) {
    pthread_cond_wait(&sync_threads->cond_master, &sync_threads->mutex);
}
pthread_mutex_unlock(&sync_threads->mutex);
}

// El maestro avisa que el nuevo tic ha comenzado y las admisiones están listas
void signal_master_done(SyncThreads *sync_threads, int tick) {
pthread_mutex_lock(&sync_threads->mutex);
sync_threads->current_tick = tick;
sync_threads->master_ready = 1;
sync_threads->worker_ready = 0;
pthread_cond_broadcast(&sync_threads->cond_worker);
pthread_mutex_unlock(&sync_threads->mutex);
}

void destroy_sync(SyncThreads *sync_threads) { //Destruye y libera los recursos, si ha terminado la simulacion
if (!sync_threads) return;
pthread_mutex_destroy(&sync_threads->mutex);
pthread_cond_destroy(&sync_threads->cond_master);
pthread_cond_destroy(&sync_threads->cond_worker);
}