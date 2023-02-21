#include<stdio.h>
#include <stdlib.h>
#include "malloc.h"

int main(){
  
  int *p= (int)pm_malloc(100*sizeof(int));
  char *q= (char)pm_malloc(250*sizeof(char));
  int *r= (int)pm_malloc(1000*sizeof(int));
  
  pm_free(p);
  char *w= (char)pm_malloc(700);
  
  pm_free(r);
  int *k= (int)pm_malloc(500*sizeof(int));
  printf("Allocation and deallocation is done successfully!");
  return 0;
 
}
