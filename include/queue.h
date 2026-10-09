#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "tcb.h"

#define QUANTUM_0 2
#define QUANTUM_1 4
#define QUANTUM_2 -1 //Se maneja con un FIFO




typedef struct Node{

    tcb_t* process;
    struct Node* next;

} node_t;


typedef struct {

node_t* head;
node_t* tail;
int size;
int quantum;


} queue_t;

void enqueue(queue_t* q, tcb_t* process);
void enqueueFront(queue_t* q, tcb_t* process);

tcb_t* dequeue(queue_t* q);
tcb_t* front(queue_t* q);

queue_t* initQueue( int quantum);
int isEmpty(queue_t* q);
void destroyQueue(queue_t* q);
void printQueue(queue_t* q);


#endif 
