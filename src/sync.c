#include "../include/sync.h"
#include <stdlib.h>
#include <stdio.h>

void init_sync(SystemSync *sync_sys) {
    if (!sync_sys) return;
    pthread_mutex_init(&sync_sys->mutex, NULL);
    pthread_cond_init(&sync_sys->cond_master, NULL);
    pthread_cond_init(&sync_sys->cond_worker, NULL);
    sync_sys->current_tick = 0;
    sync_sys->master_ready = 0;
    sync_sys->worker_ready = 0;
    sync_sys->simulation_finished = 0;
}

void destroy_sync(SystemSync *sync_sys) {
    if (!sync_sys) return;
    pthread_mutex_destroy(&sync_sys->mutex);
    pthread_cond_destroy(&sync_sys->cond_master);
    pthread_cond_destroy(&sync_sys->cond_worker);
}

// El trabajador espera a que el maestro termine de admitir las tareas del tic actual
void wait_for_master(SystemSync *sync_sys, int tick) {
    pthread_mutex_lock(&sync_sys->mutex);
    while (sync_sys->current_tick == tick && !sync_sys->master_ready && !sync_sys->simulation_finished) {
        pthread_cond_wait(&sync_sys->cond_worker, &sync_sys->mutex);
    }
    pthread_mutex_unlock(&sync_sys->mutex);
}

// El trabajador avisa que ha completado su procesamiento para el tic actual
void signal_worker_done(SystemSync *sync_sys, int tick) {
    pthread_mutex_lock(&sync_sys->mutex);
    sync_sys->worker_ready = 1;
    pthread_cond_signal(&sync_sys->cond_master);
    pthread_mutex_unlock(&sync_sys->mutex);
}

// El maestro espera a que el trabajador procese el tic actual antes de avanzar al siguiente
void wait_for_worker(SystemSync *sync_sys, int tick) {
    pthread_mutex_lock(&sync_sys->mutex);
    while (sync_sys->current_tick == tick && !sync_sys->worker_ready && !sync_sys->simulation_finished) {
        pthread_cond_wait(&sync_sys->cond_master, &sync_sys->mutex);
    }
    pthread_mutex_unlock(&sync_sys->mutex);
}

// El maestro avisa que el nuevo tic ha comenzado y las admisiones están listas
void signal_master_done(SystemSync *sync_sys, int tick) {
    pthread_mutex_lock(&sync_sys->mutex);
    sync_sys->current_tick = tick;
    sync_sys->master_ready = 1;
    sync_sys->worker_ready = 0;
    pthread_cond_broadcast(&sync_sys->cond_worker);
    pthread_mutex_unlock(&sync_sys->mutex);
}