#include "../include/queue.h"

void enqueue(queue_t *q, tcb_t *process){

    if(q == NULL || process == NULL) return;

    node_t* newNode = (node_t*) malloc(sizeof(node_t));
    if(newNode == NULL) return;
    newNode->process = process;
    newNode->next = NULL;

    if(isEmpty(q)){
        q->head = newNode;
        q->tail = q->head;
    }
    else{
        q->tail->next = newNode;
        q->tail = newNode;
    }

    q->size++;



}
void enqueueFront(queue_t* q, tcb_t* process){


    if(q == NULL || process == NULL) return;

        node_t* newNode = (node_t*) malloc(sizeof(node_t));
        if(newNode == NULL) return;
        newNode->process = process;
        newNode->next = NULL;

        if(isEmpty(q)){
            q->head = newNode;
            q->tail = q->head;
        }
        else{
            newNode->next = q->head;
            q->head = newNode;
        }
        q->size++;

}

tcb_t* dequeue(queue_t* q){

    if(isEmpty(q)) return NULL;

    node_t *nodeAux = q->head;
    tcb_t *process = nodeAux->process;

    q->head = nodeAux->next;
    if(q->head == NULL){
        q->tail = NULL;
    }

    free(nodeAux);
    q->size--;

    return process;

}

tcb_t* front(queue_t* q){

    if(isEmpty(q)) return NULL;
    return q->head->process;
}

queue_t* initQueue(int quantum){
    
    queue_t* q = (queue_t*) malloc(sizeof(queue_t));   

    if(q == NULL) return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->quantum = quantum;
    q->size = 0;

    
    return q;

}
int isEmpty(queue_t* q){


    if(q == NULL || q->head == NULL){
        return 1;
    }

    return 0;

}
void destroyQueue(queue_t* q){

    if (q == NULL) return;

    while (!isEmpty(q))
        {
            dequeue(q);
        }
    free(q);

}

void printQueue(queue_t* q) {
    if (q == NULL || isEmpty(q)) {
        printf("[ cola vacia ]");
        return;
    }

    node_t* aux = q->head;
    printf("[ ");
    while (aux != NULL) {
        if (aux->process != NULL) {
            tcbPrint(aux->process);
        }
        aux = aux->next;
    }
    printf("]");
}