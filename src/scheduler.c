#include "../include/scheduler.h"


scheduler_t* schedulerInit(){

    scheduler_t* scheduler = (scheduler_t*) malloc(sizeof(scheduler_t));

    if(scheduler == NULL) return NULL;

    scheduler->timer = 0;

    scheduler->queue0 = (initQueue(QUANTUM_0));
    scheduler->queue1 = (initQueue(QUANTUM_1));
    scheduler->queue2 = (initQueue(QUANTUM_2));

    scheduler->currentProcess = NULL;
    scheduler->lastEnd = NULL;
    scheduler->lastChavez = NULL;
    scheduler->lastExpirated = NULL;
    scheduler->lastDispatched = NULL;

    if ( scheduler->queue0 == NULL || scheduler->queue1 == NULL || scheduler->queue2 == NULL) 
        {
        schedulerDestroy(scheduler);
        return NULL;
        }

    return scheduler;

}

void schedulerDestroy(scheduler_t* scheduler){

    if(scheduler == NULL) return;

    destroyQueue(scheduler->queue0);
    destroyQueue(scheduler->queue1);
    destroyQueue(scheduler->queue2);

    free(scheduler);


    return;
}

scheduler_t* schedulerAdmit(scheduler_t* scheduler, tcb_t* newProcess){

    if(scheduler == NULL || newProcess == NULL) return NULL;

    int p = newProcess->priority;

    if(p == 0){
        enqueue(scheduler->queue0, newProcess);
    }
    else if(p == 1){
        enqueue(scheduler->queue1, newProcess);

    }
    else if(p == 2){
        enqueue(scheduler->queue2, newProcess);
    }

    newProcess->execState = READY;

    
    return scheduler;

}

tcb_t* schedulerExpiration(scheduler_t* scheduler){

    if(scheduler == NULL || scheduler->currentProcess == NULL) return NULL;

    tcb_t* current = scheduler->currentProcess;
    int esReemplazable = 0;

    if(current->priority == 0 && current->usedQuantum >= QUANTUM_0) {
        esReemplazable = 1;
    } else if (current->priority == 1 && current->usedQuantum >= QUANTUM_1) {
        esReemplazable = 1;
    }

    if(esReemplazable){

        current->usedQuantum = 0;
        current->execState = INTERRUPTED;

        if(current->priority == 0){
            enqueue(scheduler->queue0, current);
        }
        else if( current->priority == 1){
            enqueue(scheduler->queue1, current);
        }

        scheduler->currentProcess = NULL;

        return current;

    }
    return NULL;
}

tcb_t*  schedulerChavez(scheduler_t* scheduler){

    if (scheduler == NULL || scheduler->currentProcess == NULL) return NULL;

    tcb_t* current = scheduler->currentProcess;
    int chavez = 0;

    if (current->priority > 0 && !isEmpty(scheduler->queue0)) {
        chavez = 1;
    } 

    else if (current->priority > 1 && !isEmpty(scheduler->queue1)) {
        chavez = 1;
    }

    if (chavez) {

        current->execState = INTERRUPTED;
        
        if (current->priority == 1){
            enqueueFront(scheduler->queue1, current);
        }      
        else if (current->priority == 2){
            enqueueFront(scheduler->queue2, current);
        }

        scheduler->currentProcess = NULL; 
        return current;
    }
    return NULL;
}

void schedulerDispatch(scheduler_t* scheduler){

    if (scheduler == NULL || scheduler->currentProcess != NULL) return;

    if(!isEmpty(scheduler->queue0)){
        scheduler->currentProcess = dequeue(scheduler->queue0);
    } else if (!isEmpty(scheduler->queue1)) {
        scheduler->currentProcess = dequeue(scheduler->queue1);
    } else if (!isEmpty(scheduler->queue2)) {
        scheduler->currentProcess = dequeue(scheduler->queue2);
    }

    if(scheduler->currentProcess != NULL){
        scheduler->currentProcess->execState = RUNNING;
        scheduler->lastDispatched = scheduler->currentProcess;
    }

    
}

void schedulerMain(scheduler_t* scheduler){

    if(scheduler == NULL) return;

    scheduler->lastExpirated = NULL;
    scheduler->lastChavez = NULL;
    scheduler->lastDispatched = NULL;

    scheduler->lastExpirated = schedulerExpiration(scheduler);
    scheduler->lastChavez = schedulerChavez(scheduler);
    schedulerDispatch(scheduler);

  


}

void schedulerExecTick(scheduler_t* scheduler){

    scheduler->lastEnd = NULL;

    if(scheduler == NULL) return;

     if(scheduler->currentProcess != NULL){
        scheduler->currentProcess->remainingBurst--;
        scheduler->currentProcess->usedQuantum++;

        if(scheduler->currentProcess->remainingBurst == 0){
            scheduler->lastEnd = scheduler->currentProcess;
            scheduler->currentProcess->execState = FINISHED;
            scheduler->currentProcess->returnTime = scheduler->timer + 1;
            scheduler->currentProcess = NULL;
        }
    }
    schedulerWaitIncrement(scheduler->queue0);
    schedulerWaitIncrement(scheduler->queue1);
    schedulerWaitIncrement(scheduler->queue2);
}


void schedulerWaitIncrement(queue_t* queue){

    if(queue == NULL) return;
    node_t* aux = queue->head;
    while(aux != NULL){
        if(aux->process != NULL){
            aux->process->waitTime++;
        }
        aux = aux->next;
    } 

}

void printNormal(scheduler_t* scheduler) {
    if (scheduler == NULL) return; 

    if (scheduler->lastExpirated != NULL && scheduler->lastExpirated != scheduler->currentProcess) {
        int q = (scheduler->lastExpirated->priority == 0) ? QUANTUM_0 : QUANTUM_1;
        printf("[T-%04d] QUANTUM_EXPIRED: Sentinel %d yields CPU via RR rotation (Q=%d)\n", 
               scheduler->timer, scheduler->lastExpirated->id, q);
    }

    if (scheduler->lastEnd != NULL) {
        printf("[T-%04d] COMPLETED: Sentinel ID: %d -> Turnaround Time: %d ticks\n", 
               scheduler->lastEnd->returnTime, 
               scheduler->lastEnd->id, 
               scheduler->lastEnd->returnTime - scheduler->lastEnd->arrivalTime);
    }
}

void printVerbose(scheduler_t* scheduler) {
    if (scheduler == NULL) return;

    printNormal(scheduler);

    
    printf("--- [ Estado del Sistema en Tg = %04d ] ---\n", scheduler->timer);
    
    printf("  CPU_0 : ");
    if (scheduler->currentProcess != NULL) {
        tcbPrint(scheduler->currentProcess);
        printf("\n");
    } else {
        printf("[ ociosa ]\n");
    }

    printf("  Cola 0 (RR Q=2) : "); printQueue(scheduler->queue0); printf("\n");
    printf("  Cola 1 (RR Q=4) : "); printQueue(scheduler->queue1); printf("\n");
    printf("  Cola 2 (FCFS)   : "); printQueue(scheduler->queue2); printf("\n");
    printf("-------------------------------------------\n");
}












