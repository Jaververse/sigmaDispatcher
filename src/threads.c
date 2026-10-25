#include "../include/threads.h"
#include "../include/sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Hilo Maestro, Controla el reloj discreto Tg y la admisión de nuevas tareas que lleguen
void *master_threadF(void *arg) {
    threadAdmin *manager = (threadAdmin *)arg;
    
    while (1) { //Bucle para avanzar tic a tic mientras se completa la simulación
        pthread_mutex_lock(&manager->sync_threads.mutex);

        // Verificar si ya se completaron todas las tareas, ya que de ser asi cerramos el bucle y despertamos al trabajador
        if (manager->completed_tasks >= manager->num_tasks) {
        manager->sync_threads.simulation_finished = 1;
        manager->sync_threads.master_ready = 1;
        pthread_cond_signal(&manager->sync_threads.cond_worker);
        pthread_mutex_unlock(&manager->sync_threads.mutex);
        break;
        }

        int t = manager->sync_threads.current_tick; //tomamos el tic global actual
        manager->scheduler->timer = t; // Sincronizar el reloj del planificador al que acabamos de obtener
        pthread_mutex_unlock(&manager->sync_threads.mutex);

        //Fase para admitir
        for (int i = 0; i < manager->num_tasks; i++) {
            if (manager->tasks[i]->execState == NEW && manager->tasks[i]->arrivalTime == t) { // Recorriendo todas las tareas si encontramos una que llega en este tic.
            schedulerAdmit(manager->scheduler, manager->tasks[i]); // Admitimos llamando al schedulerAdmit
            const char* tNames[] = {"Command","Driller","Heavy"};
            printf("[T-%04d] ARRIVAL: Sentinel ID: %d [Type: %s]\n",t, manager->tasks[i]->id, tNames[manager->tasks[i]->priority]); // Imprimimos la llegada
            }
        }

        // LLamamos al hilo trabajador ya que el maestro complete las admisiones de este tic
        pthread_mutex_lock(&manager->sync_threads.mutex);
        manager->sync_threads.master_ready = 1;
        manager->sync_threads.worker_ready = 0;
        pthread_cond_signal(&manager->sync_threads.cond_worker);

        // Esperar a que el hilo trabajador (CPU_0) procese el tic actual, nos bloqueamos.
        while (!manager->sync_threads.worker_ready && !manager->sync_threads.simulation_finished) {
        pthread_cond_wait(&manager->sync_threads.cond_master, &manager->sync_threads.mutex);
        }
        pthread_mutex_unlock(&manager->sync_threads.mutex);
    }

    return NULL;
}

// Hilo Trabajador es el nucleo fisico CPU_0 ejecutando las tareas
void *worker_threadF(void *arg) {
    threadAdmin *manager = (threadAdmin *)arg;
    
    while (1) {
        pthread_mutex_lock(&manager->sync_threads.mutex);

        // Esperar a que el hilo maestro indique que hay un tic listo
        while (!manager->sync_threads.master_ready && !manager->sync_threads.simulation_finished) {
            pthread_cond_wait(&manager->sync_threads.cond_worker, &manager->sync_threads.mutex);
        }

        if (manager->sync_threads.simulation_finished) { // Si la simulacion termino, salimos del bucle.
        pthread_mutex_unlock(&manager->sync_threads.mutex);
        break;
        }

        int t = manager->sync_threads.current_tick; // tomamos el tic global actual
        manager->scheduler->timer = t;
        pthread_mutex_unlock(&manager->sync_threads.mutex);

        if (manager->scheduler->lastEnd != NULL) { // comprobamos si en el tic anterior finalizo alguna tarea, ya que si esto pasa, imprime el mensaje de completed.
        printf("[T-%04d] COMPLETED: Sentinel ID: %d -> Turnaround Time: %d ticks\n", manager->scheduler->lastEnd->returnTime, manager->scheduler->lastEnd->id, manager->scheduler->lastEnd->returnTime - manager->scheduler->lastEnd->arrivalTime);
        manager->completed_tasks++; // aumentamos el contador de completadas.
        manager->scheduler->lastEnd = NULL; // reiniciamos el puntero
        }

        // Verificamos si se terminaron todas las tareasy y si es asi finalizamos la simulación
        if (manager->completed_tasks >= manager->num_tasks) {
        pthread_mutex_lock(&manager->sync_threads.mutex);
        manager->sync_threads.simulation_finished = 1;
        manager->sync_threads.worker_ready = 1;
        pthread_cond_signal(&manager->sync_threads.cond_master);
        pthread_mutex_unlock(&manager->sync_threads.mutex);
        break;
        }

        // Pasamos a ejecutar las fases del planificador.
        schedulerMain(manager->scheduler); 

        if (manager->verbose_mode) {
        printVerbose(manager->scheduler);
        } else {
        printNormal(manager->scheduler);
        }

        schedulerExecTick(manager->scheduler); // Ejecutamos el tick actual, sobre una tarea activa

        // Aumentamos el tic, y pasamos a darle el control al maestro
        pthread_mutex_lock(&manager->sync_threads.mutex);
        manager->sync_threads.current_tick++;
        manager->scheduler->timer = manager->sync_threads.current_tick;
        manager->sync_threads.master_ready = 0;
        manager->sync_threads.worker_ready = 1;
        pthread_cond_signal(&manager->sync_threads.cond_master);
        pthread_mutex_unlock(&manager->sync_threads.mutex);
    }

    return NULL;
}
//Funciones para gestionar los hilos
int start_thread_manager(threadAdmin *manager, scheduler_t *scheduler, tcb_t **tasks, int num_tasks, int verbose) {
    if (!manager) return -1;
    //asignamos los parametros de configuracion
    manager->verbose_mode = verbose;
    manager->scheduler = scheduler;
    manager->tasks = tasks;
    manager->num_tasks = num_tasks;
    manager->completed_tasks = 0;

    init_sync(&manager->sync_threads);

    if (pthread_create(&manager->master_thread, NULL, master_threadF, manager) != 0) { //Creamos el hilo maestro
    perror("Error al crear el Hilo Maestro");
    destroy_sync(&manager->sync_threads);
    return -1;
    }

    if (pthread_create(&manager->worker_thread, NULL, worker_threadF, manager) != 0) { //Creamos el hilo trabajador
    perror("Error al crear el Hilo Trabajador");
    pthread_cancel(manager->master_thread);
    destroy_sync(&manager->sync_threads);
    return -1;
    }

    return 0;
}

void join_thread_manager(threadAdmin *manager) { // esperamos que los hilos terminen su trabajo, para liberar.
    if (!manager) return;
    pthread_join(manager->master_thread, NULL);
    pthread_join(manager->worker_thread, NULL);
    destroy_sync(&manager->sync_threads);
}