#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include "sync.h"
#include "scheduler.h"
#include "tcb.h"

// Contexto compartido global para la ejecución de los hilos del simulador, aqui uno todos el sistema
typedef struct {
pthread_t master_thread; //Hilo maestro
pthread_t worker_thread; //Hilo trabajador
SyncThreads sync_threads; //tengo el mutex y variables para la coordinacion de los hilos
int verbose_mode; // para el -v
    
// Punteros y contadores para trabajar los hilos y las tareas.
scheduler_t *scheduler;
tcb_t **tasks;
int num_tasks;
int completed_tasks;
} threadAdmin; 

// Funciones para iniciar, gestionar y finalizar los hilos 
int start_thread_manager(threadAdmin *manager, scheduler_t *scheduler, tcb_t **tasks, int num_tasks, int verbose);
void *master_thread_routine(void *arg);
void *worker_thread_routine(void *arg);
void join_thread_manager(threadAdmin *manager);

#endif