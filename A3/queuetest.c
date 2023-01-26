#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#include "queue.h"

#define MAX_NUMBER_OF_PROCESS    7    //maximum processes
//
// Created by chiem on 1/24/2023.
//

// int unitTest1(int status){
//     printf("========queue unitTest1========\n");
//     queue_t* test1 = makeQueue();
//     for(int i = 1; i <= 3; i++){
//         struct queue *p = malloc(sizeof(*p));
//         p->head = p->tail = i;
//         enqueue(test1,p);
//         dequeue(test1);
//         printf("one node removed = (%d , %d), Size is %d\n", p->head, p->tail, test1->size);
//     }
    

//     free(test1);
//     return 1;
// }
// int unitTest2(int status){
//     printf("========queue unitTest2========\n");
//     queue_t* test2 = makeQueue();
//     for(int i = 1; i <= 5; i++){
//         struct queue *p = malloc(sizeof(*p));
//         enqueue(test2,p);
//         enqueue(test2,p);
//         enqueue(test2,p);
//         enqueue(test2,p);
//         enqueue(test2,p);
//         printf("one node added = (%d , %d), Size is %d\n", p->head, p->tail, test2->size);
//     }
    
//     free(test2);
//     return 1;
// }
// int unitTest3(int status){}
// int unitTest4(int status){}

// int (*unitTests[])(int)={
//         unitTest1,
//         unitTest2,
//         NULL
// };

int main(void){
    queue_t* test1 = makeQueue();

    printf("\n");
    printf("##########################\n");
    printf("#  Queue before Enqueue  #\n");
    printf("##########################\n");
    printf("the size of queue: %d\n", test1->size);

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
    printdata(test1->head->val);
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
    printdata(test1->head->val);
    }

    free(test1);
    return 0;
//     unsigned int testsPassed = 0;
//     // List of Unit Tests to test your data structure
//     int counter =0;
//     while(unitTests[counter]!=NULL){
//         unitTests[counter](1);
//         counter++;
//     }

//     return 0;
}

