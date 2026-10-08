#include "../include/tcb.h"

tcb_t* tcbInit(int id, int arrivalTime, int priority, int totalBurst){

    tcb_t* process = (tcb_t*) malloc(sizeof(tcb_t));
    if(process == NULL){
        return NULL;
    }

    process->arrivalTime = arrivalTime;
    process->priority = priority;
    process->id = id;
    process->totalBurst = totalBurst;
    process->execState = NEW;
    process->remainingBurst = totalBurst;
    process->usedQuantum = 0;
    process->waitTime = 0;
    process->returnTime = 0;

    return process;
}

void tcbDestroy(tcb_t* process){

    if(process != NULL){
        free(process);
    }
}
