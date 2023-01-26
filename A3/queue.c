#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#include "queue.h"

//
// Created by chiem on 1/24/2023.
//

//Define a type
// process_t that represents a process that has both an identifier (int),
//a name (char *), and a priority (int). Implement this as a struct.
// (typedef called process_t) inside has a int identifier and a char *name and int priority)
//implement a struct

//Define a type
// queue_t that represents a queue that can hold anything
//(Hint: the data in the node must be of type void *).
// Implement this as a struct.

//struct queue{
//    unsigned int back;	    // The next free position in the queue
//    // (i.e. the end or tail of the line)
//    unsigned int front;	    // Current 'head' of the queue
//    // (i.e. the front or head of the line)
//    unsigned int size;	    // How many total elements we currently have enqueued.
//    unsigned int capacity;  // Maximum number of items the queue can hold
//    int* data; 		          // The 'integer' data our queue holds
//};
//typedef struct queue process_t;


//Define a function enqueue(queue_t* queue, void* element) that can adds element to the end of the queue.
//You may implement the queue as a linked list or as an array.

//If you implement it as a linked list then you might want to add forward (and perhaps backward) pointers to the queue_t structure defined above.
//Define a function void* dequeue(queue_t* queue) that removes and
//returns the data within the element at the front of the queue.

//Define a function process_t* dequeueProcess(queue_t* queue) that removes
//and returns the process with the highest priority in the queue.
//Note that we are presuming process_t types for this function but not the others.

//Define a function int qsize(queue_t* queue) that returns the number of elements in the queue;
//0 if the queue is empty.



//struct process{
//    unsigned int identifier;
//    char * name;
//    unsigned int priority;
//
//};
//
//typedef struct process process_t;

// typedef struct node{
//     node_t *next;
//     node_t *prev;
//     void *val;
// }node_t;



//struct queue{
//    struct node_t *head; // Current 'head' of the queue
//    struct node_t *tail; // The next free position in the queue
//    int size; // How many total elements we currently have enqueued.
//    unsigned int capacity;  // Maximum number of items the queue can hold
//    int* data; 		          // The 'integer' data our queue holds
//};
//
//typedef struct queue queue_t;

queue_t *makeQueue() {
    queue_t *queue = (queue_t *)malloc(sizeof (queue_t));
    if (queue == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}


int enqueue(queue_t* queue, void* element){
    struct node *newNode = (struct node*)malloc(sizeof(struct node));
    if(newNode == NULL){
        return -1;
    }
    newNode->val = element;

    if(queue->size == 0){ //queue empty
        newNode->next = newNode;
        newNode->prev = newNode;
        queue->head = newNode;
        queue->tail = newNode;

    }else{
        newNode->next = queue->tail;
        newNode->prev = queue->head;
        queue->tail->prev = newNode;
        queue->head->next = newNode;
        queue->tail = newNode;
    }
    queue->size++;

    return 0;

}



//void enqueue(queue_t* queue, node_t element){
//    if (queue->tail == NULL) { // queue is empty
//        queue->head = element.val;
//    } else { // queue not empty
//        queue->tail->next = element.val;
//    }
//    queue->tail = new_node;
//    queue->size++;
//
//}



void* dequeue(queue_t* queue){
    if( queue->head == queue->tail ) { // only one item
        queue->tail = NULL;
    }
    if( queue->size == 0 ) { // size is zero
        return NULL;
    }

    void* res = queue->head->val;

    struct node * oldNode = queue->head;
    oldNode->prev->next = queue->tail;
    queue->tail->prev = oldNode->prev;
    queue->head = oldNode->next;


    queue->size--;
    return res;

}

void printdata(node_t* head)
{
    process_t *p = head;
    if (head == NULL) {
        printf("NULL\n");
    }
    else {
        printf("%s->", p->name);
        printdata(head->next);
    }
}

process_t* dequeueProcess(queue_t* queue){
    //Define a function process_t* dequeueProcess(queue_t* queue) (done)
    // that removes and returns the process with the highest priority in the queue.
    //Note that we are presuming process_t types for this function but not the others.
    node_t *temp;
    process_t* n = queue->head->val;
    temp = queue->head;
    queue->head = queue->head->next;
    temp->val = queue->head;
    free(temp);
    return n;

}

//int qempty(queue_t* queue){
//
//    return 0;
//}

int qsize(queue_t* queue){
    if(queue->head == NULL){
        return 0;
    } else
        return queue->size;
}



