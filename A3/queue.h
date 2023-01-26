#include <stdlib.h>

#ifndef QUEUE_H
#define QUEUE_H

#endif

//
// Created by chiem on 1/24/2023.
//

//Defined a type process_t that represents a process that has both an identifier (int),
//a name (char *), and a priority (int). Implement this as a struct.
// (typedef called process_t) inside has a int identifier and a char *name and int priority)
//implemented a struct

struct process{
    unsigned int identifier;
    char *name;
    unsigned int priority;

};

typedef struct process process_t;


//Defined a type node_t that represents a node
//(the val in the node must be of type void *)
typedef struct node{
    struct node* next;
    struct node* prev;
    void *val;
    process_t* p;
}node_t;


//Defined a type queue_t that represents a queue that can hold anything
//(Hint: the data in the node must be of type void *).
// Implemented this as a struct.


struct queue{
    struct node* head; // Current 'head' of the queue
    struct node* tail; // The next free position in the queue
    int size; // How many total elements we currently have enqueued.
    void *data;
};

typedef struct queue queue_t;

queue_t *makeQueue();
void enqueue(queue_t* queue, void* element);
void* dequeue(queue_t* queue);
process_t* dequeueProcess(queue_t* queue);
int qsize(queue_t* queue);
void printdata(node_t *head);
