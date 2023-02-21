#include<stdio.h>
#include <stdlib.h>
#include "malloc.h"

int main(){
  
  int *p= (int)myMalloc(100*sizeof(int));
  char *q= (char)myMalloc(250*sizeof(char));
  int *r= (int)myMalloc(1000*sizeof(int));
  
  myFree(p);
  char *w= (char)myMalloc(700);
  
  myFree(r);
  int *k= (int)myMalloc(500*sizeof(int));
  printf("Allocation and deallocation is done successfully!");
  return 0;
 
}
