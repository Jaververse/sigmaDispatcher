#include "../include/threads.h"
#include "../include/sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Rutina del Hilo Maestro: Controla el reloj discreto (T_g) y la admisión de llegadas
void *master_thread_routine(void *arg) {
    ThreadManager *manager = (ThreadManager *)arg;
    
    while (1) {
        pthread_mutex_lock(&manager->sync_sys.mutex);

        // Verificar si ya se completaron todas las tareas
        if (manager->completed_tasks >= manager->num_tasks) {
            manager->sync_sys.simulation_finished = 1;
            manager->sync_sys.master_ready = 1;
            pthread_cond_signal(&manager->sync_sys.cond_worker);
            pthread_mutex_unlock(&manager->sync_sys.mutex);
            break;
        }

        int t = manager->sync_sys.current_tick;
        manager->scheduler->timer = t; // Sincronizar el reloj oficial del planificador
        pthread_mutex_unlock(&manager->sync_sys.mutex);

        //FASE DE ADMISIÓN: Evaluar y registrar llegadas exactamente en el tic actual 't'
        for (int i = 0; i < manager->num_tasks; i++) {
            if (manager->tasks[i]->execState == NEW && manager->tasks[i]->arrivalTime == t) {
                schedulerAdmit(manager->scheduler, manager->tasks[i]);
                const char* typeNames[] = {"Command", "Driller", "Heavy"};
                printf("[T-%04d] ARRIVAL: Sentinel ID: %d [Type: %s]\n", 
                       t, manager->tasks[i]->id, typeNames[manager->tasks[i]->priority]);
            }
        }

        // Notificar al hilo trabajador que el maestro completó las admisiones de este tic
        pthread_mutex_lock(&manager->sync_sys.mutex);
        manager->sync_sys.master_ready = 1;
        manager->sync_sys.worker_ready = 0;
        pthread_cond_signal(&manager->sync_sys.cond_worker);

        // Esperar a que el hilo trabajador (CPU_0) procese el tic actual
        while (!manager->sync_sys.worker_ready && !manager->sync_sys.simulation_finished) {
            pthread_cond_wait(&manager->sync_sys.cond_master, &manager->sync_sys.mutex);
        }
        pthread_mutex_unlock(&manager->sync_sys.mutex);
    }

    return NULL;
}

// Rutina del Hilo Trabajador: Representa el núcleo físico CPU_0 ejecutando las tareas
void *worker_thread_routine(void *arg) {
    ThreadManager *manager = (ThreadManager *)arg;
    
    while (1) {
        pthread_mutex_lock(&manager->sync_sys.mutex);

        // Esperar a que el Hilo Maestro esté listo con el tic actual
        while (!manager->sync_sys.master_ready && !manager->sync_sys.simulation_finished) {
            pthread_cond_wait(&manager->sync_sys.cond_worker, &manager->sync_sys.mutex);
        }

        if (manager->sync_sys.simulation_finished) {
            pthread_mutex_unlock(&manager->sync_sys.mutex);
            break;
        }

        int t = manager->sync_sys.current_tick;
        manager->scheduler->timer = t;
        pthread_mutex_unlock(&manager->sync_sys.mutex);

        //COMPROBAR FINALIZACIONES PENDIENTES
        if (manager->scheduler->lastEnd != NULL) {
            printf("[T-%04d] COMPLETED: Sentinel ID: %d -> Turnaround Time: %d ticks\n", 
                   manager->scheduler->lastEnd->returnTime, 
                   manager->scheduler->lastEnd->id, 
                   manager->scheduler->lastEnd->returnTime - manager->scheduler->lastEnd->arrivalTime);
            manager->completed_tasks++;
            manager->scheduler->lastEnd = NULL;
        }

        // Si ya terminaron todas las tareas, finalizar simulación
        if (manager->completed_tasks >= manager->num_tasks) {
            pthread_mutex_lock(&manager->sync_sys.mutex);
            manager->sync_sys.simulation_finished = 1;
            manager->sync_sys.worker_ready = 1;
            pthread_cond_signal(&manager->sync_sys.cond_master);
            pthread_mutex_unlock(&manager->sync_sys.mutex);
            break;
        }

        // EJECUTAR LAS FASES DEL PLANIFICADOR (Integrante 1)
        schedulerMain(manager->scheduler); 

        if (manager->verbose_mode) {
            printVerbose(manager->scheduler);
        } else {
            printNormal(manager->scheduler);
        }

        schedulerExecTick(manager->scheduler);

        // AVANZAR AL SIGUIENTE TIC Y NOTIFICAR AL MAESTRO
        pthread_mutex_lock(&manager->sync_sys.mutex);
        manager->sync_sys.current_tick++;
        manager->scheduler->timer = manager->sync_sys.current_tick;
        manager->sync_sys.master_ready = 0;
        manager->sync_sys.worker_ready = 1;
        pthread_cond_signal(&manager->sync_sys.cond_master);
        pthread_mutex_unlock(&manager->sync_sys.mutex);
    }

    return NULL;
}

int start_thread_manager(ThreadManager *manager, scheduler_t *scheduler, tcb_t **tasks, int num_tasks, int verbose) {
    if (!manager) return -1;
    
    manager->verbose_mode = verbose;
    manager->scheduler = scheduler;
    manager->tasks = tasks;
    manager->num_tasks = num_tasks;
    manager->completed_tasks = 0;

    init_sync(&manager->sync_sys);

    if (pthread_create(&manager->master_thread, NULL, master_thread_routine, manager) != 0) {
        perror("Error al crear el Hilo Maestro");
        destroy_sync(&manager->sync_sys);
        return -1;
    }

    if (pthread_create(&manager->worker_thread, NULL, worker_thread_routine, manager) != 0) {
        perror("Error al crear el Hilo Trabajador");
        pthread_cancel(manager->master_thread);
        destroy_sync(&manager->sync_sys);
        return -1;
    }

    return 0;
}

void join_thread_manager(ThreadManager *manager) {
    if (!manager) return;
    pthread_join(manager->master_thread, NULL);
    pthread_join(manager->worker_thread, NULL);
    destroy_sync(&manager->sync_sys);
}