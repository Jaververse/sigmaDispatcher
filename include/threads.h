#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include "sync.h"
#include "scheduler.h"
#include "tcb.h"

// Contexto compartido para la ejecución de los hilos del simulador
typedef struct {
    pthread_t master_thread;
    pthread_t worker_thread;
    SystemSync sync_sys;
    int verbose_mode;
    
    // Punteros necesarios para conectar con el Integrante 1 y el Integrante 3
    scheduler_t *scheduler;
    tcb_t **tasks;
    int num_tasks;
    int completed_tasks;
} ThreadManager;

// Funciones para iniciar, gestionar y finalizar los hilos POSIX
int start_thread_manager(ThreadManager *manager, scheduler_t *scheduler, tcb_t **tasks, int num_tasks, int verbose);
void *master_thread_routine(void *arg);
void *worker_thread_routine(void *arg);
void join_thread_manager(ThreadManager *manager);

#endif // THREADS_H