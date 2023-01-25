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
    enqueue(test1,1);
    dequeue(test1);

    //free_queue(test1);
    return 1;
}
int unitTest2(int status){
    printf("========queue unitTest2========\n");
    queue_t* test2 = makeQueue();
    enqueue(test2,1);
    enqueue(test2,2);
    enqueue(test2,3);
    enqueue(test2,4);
    enqueue(test2,5);
    //free_queue(test7);
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

