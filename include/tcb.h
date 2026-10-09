#ifndef TCB_H
#define TCB_H


#include <stdlib.h>
#include <stdio.h>

typedef enum {

    NEW,
    READY,
    RUNNING,
    INTERRUPTED,
    FINISHED
} States;

typedef struct
{
    int id; 
    States execState;
    int priority;
    int arrivalTime;
    int totalBurst;
    int remainingBurst;
    int usedQuantum;
    int waitTime;
    int returnTime;
} tcb_t;

tcb_t* tcbInit( int id, int arrivalTime, int priority, int totalBurst);
void tcbDestroy(tcb_t* process);
void tcbPrint(tcb_t* process);

#endif
