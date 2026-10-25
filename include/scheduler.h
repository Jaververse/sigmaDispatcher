#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include <stdlib.h>


typedef struct {

queue_t* queue0;
queue_t* queue1;
queue_t* queue2;

tcb_t* currentProcess;
int timer;

tcb_t* lastEnd;
tcb_t* lastChavez;
tcb_t* lastExpirated;
tcb_t* lastDispatched;

} scheduler_t;

scheduler_t* schedulerInit();
void schedulerDestroy(scheduler_t* scheduler);
scheduler_t* schedulerAdmit(scheduler_t *scheduler, tcb_t* newProcess);
tcb_t* schedulerExpiration(scheduler_t* scheduler);
tcb_t* schedulerChavez(scheduler_t* scheduler);
void schedulerDispatch(scheduler_t* scheduler);
void schedulerMain(scheduler_t* scheduler);
void schedulerExecTick(scheduler_t* scheduler);
void schedulerWaitIncrement(queue_t* queue);



void printVerbose(scheduler_t* scheduler);
void printNormal(scheduler_t* scheduler);


#endif