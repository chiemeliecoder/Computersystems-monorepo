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
    unsigned int identifier; //ID of the process
    char *name; //the name of tthe process
    unsigned int priority; //priority of the process

};

typedef struct process process_t;


//Defined a type node_t that represents a node
//(the val in the node must be of type void *)
typedef struct node{
    struct node* next; //next node
    struct node* prev; //previous node in the queue
    void *val; //value of the node
    process_t* p; //access the process struct
}node_t;


//Defined a type queue_t that represents a queue that can hold anything
//(Hint: the data in the node must be of type void *).
// Implemented this as a struct.


struct queue{
    struct node* head; // Current 'head' of the queue
    struct node* tail; // The next free position in the queue
    int size; // How many total elements we currently have enqueued.
    void *data; //value of the queue
};

typedef struct queue queue_t;

queue_t *makeQueue(); //make and initilize queue
void enqueue(queue_t* queue, void* element); //enqueue process
void* dequeue(queue_t* queue); //dequeue
process_t* dequeueProcess(queue_t* queue);//dequeue process
int qsize(queue_t* queue); //size of the queue
void printdata(node_t *head); //prints the process dequeued and enqueued
int isempty(queue_t *queue); //check if queue is empty
