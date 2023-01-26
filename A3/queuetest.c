#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#include "queue.h"

#define MAX_NUMBER_OF_PROCESS    7    //maximum processes
//
// Created by chiem on 1/24/2023.
//



int main(void){
    queue_t* test1 = makeQueue();

    printf("\n");
    printf("##########################\n");
    printf("#  Queue before Enqueue  #\n");
    printf("##########################\n");
    printf("the size of queue: %d\n", test1->size);
    printdata(test1->data);

    printf("\n");
    printf("##########################\n");
    printf("#         Enqueue        #\n");
    printf("##########################\n");

    process_t process[MAX_NUMBER_OF_PROCESS];
    
    for (int i=0;i<MAX_NUMBER_OF_PROCESS;i++) {
        process_t *p = &process[i];
        p->identifier = i;
        p->name = malloc(sizeof(*p->name)*9);
        sprintf(p->name, "Process%d", i);
        enqueue(test1, p);
        printf("Enqueued %s .....\t", p->name);
        printdata(test1->data);
    }

    printf("\n");
    printf("##########################\n");
    printf("#  Queue before Dequeue  #\n");
    printf("##########################\n");
    printf("the size of queue: %d\n", test1->size);
    printdata(test1->head->val);
    
 

    printf("\n");
    printf("##########################\n");
    printf("#         Dequeue        #\n");
    printf("##########################\n");

    for (int i=0;i<MAX_NUMBER_OF_PROCESS;i++) {
        process_t* dequeuedProcess =  dequeue(test1);
        printf("Dequeued %s .....\t", dequeuedProcess->name);
        printf("\nthe size of queue: %d\n", test1->size);
        //printdata(test1->head->val);
    }

    free(test1);
    return 0;
}

