#include <stdlib.h>

#ifndef QUEUE_H
#define QUEUE_H

#endif

//
// Created by chiem on 1/24/2023.
//

struct process{
    unsigned int identifier;
    char *name;
    unsigned int priority;

};

typedef struct process process_t;

typedef struct node{
    struct node* next;
    struct node* prev;
    void *val;
}node_t;




struct queue{
    struct node* head; // Current 'head' of the queue
    struct node* tail; // The next free position in the queue
    int size; // How many total elements we currently have enqueued.
};

typedef struct queue queue_t;

queue_t *makeQueue();
int enqueue(queue_t* queue, void* element);
void* dequeue(queue_t* queue);
process_t* dequeueProcess(queue_t* queue);
int qsize(queue_t* queue);
void printdata(node_t *head);
