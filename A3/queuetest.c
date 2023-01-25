#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#include "queue.h"
//
// Created by chiem on 1/24/2023.
//

int unitTest1(int status){
    printf("========queue unitTest1========\n");
    queue_t* test1 = makeQueue();
    for(int i = 1; i <= 3; i++){
        struct queue *p = malloc(sizeof(*p));
        p->head = p->tail = i;
        enqueue(test1,p);
        dequeue(test1);
        printf("one node removed = (%d , %d), Size is %d\n", p->head, p->tail, test1->size);
    }
    

    free(test1);
    return 1;
}
int unitTest2(int status){
    printf("========queue unitTest2========\n");
    queue_t* test2 = makeQueue();
    for(int i = 1; i <= 5; i++){
        struct queue *p = malloc(sizeof(*p));
        enqueue(test2,p);
        enqueue(test2,p);
        enqueue(test2,p);
        enqueue(test2,p);
        enqueue(test2,p);
        printf("one node added = (%d , %d), Size is %d\n", p->head, p->tail, test2->size);
    }
    
    free(test2);
    return 1;
}
int unitTest3(int status){}
int unitTest4(int status){}

int (*unitTests[])(int)={
        unitTest1,
        unitTest2,
        NULL
};

int main(){
    unsigned int testsPassed = 0;
    // List of Unit Tests to test your data structure
    int counter =0;
    while(unitTests[counter]!=NULL){
        unitTests[counter](1);
        counter++;
    }

    return 0;
}

