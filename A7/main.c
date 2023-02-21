#include<stdio.h>
#include <stdlib.h>
#include "malloc.h"
void unitTest1(){
   //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly. Memory is initialized then split to fit 
  int *p= (int)pm_malloc(100*sizeof(int));
  char *q= (char)pm_malloc(250*sizeof(char));
  int *r= (int)pm_malloc(1000*sizeof(int));
  
  pm_free(p);
  char *w= (char)pm_malloc(700);
  
  pm_free(r);
  int *k= (int)pm_malloc(500*sizeof(int));
  printf("Allocation and deallocation is done successfully for unitTest 1!\n");
  
}

void unitTest2(){
  //normal allocation and deallocation 
  int *p= (int)pm_malloc(100*sizeof(int));
  pm_free(p);
  char *q= (char)pm_malloc(250*sizeof(char));
  printf("Allocation and deallocation is done successfully for unitTest 2!\n");
  
}

void unitTest3(){
  //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly if not found would split it at a point it would not be able to free and would spit out Please provide a valid pointer allocated by MyMalloc when freeing
  int *p= (int)pm_malloc(100*sizeof(int));
  pm_free(p);
  char *q= (char)pm_malloc(250*sizeof(char));
  pm_free(q);
  printf("Allocation and deallocation is done successfully for unitTest 3!\n");

  
}
void unitTest4(){
  //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly if not found would split it at a point it would not be able to free and would spit out Please provide a valid pointer allocated by MyMalloc when freeing q
  int *p= (int)pm_malloc(100*sizeof(int));
   pm_free(p);
  char *q= (char)pm_malloc(250*sizeof(char));
  int *r= (int)pm_malloc(1000*sizeof(int));
  pm_free(q);
  char *w= (char)pm_malloc(700);
  int *k= (int)pm_malloc(500*sizeof(int));
  printf("Allocation and deallocation is done successfully for unitTest 4!\n");
  
}
void unitTest5(){
  //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly if not found would split it 
  int *p= (int)pm_malloc(100*sizeof(int));
   pm_free(p);
  
  printf("Allocation and deallocation is done successfully for unitTest 5!\n");
  
}

void unitTest6(){
   //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly. Memory is initialized then split to fit and would trigger no sufficient memory to allocate and return a null pointer.uncomment the code to run it,
  // int *a= (int)pm_malloc(100*sizeof(int));
  // //char *b= (char)pm_malloc(250*sizeof(char));
  // int *c= (int)pm_malloc(100*sizeof(int));
  
  // pm_free(a);
  // char *d= (char)pm_malloc(700);
  
  // pm_free(c);
  // int *e= (int)pm_malloc(500*sizeof(int));
  printf("please uncomment test 6 to test a trigger showing no sufficient memory to allocate \n");
  printf("Allocation and deallocation is done successfully for unitTest 6!\n");
  
}

int main(){
  printf("=======================unitTest1=====================\n");
	unitTest1();
  printf("=======================unitTest2=====================\n");
	unitTest2();
  printf("=======================unitTest3=====================\n");
	unitTest3();
  printf("=======================unitTest4=====================\n");
	unitTest4();
   printf("=======================unitTest5=====================\n");
	unitTest5();
  printf("=======================unitTest6=====================\n");
	unitTest6();
 
}
