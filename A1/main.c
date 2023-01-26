#include <stdio.h>
#include <stdlib.h>
#include "EzeokekeC.CS5600.LearnC.h"

/*Ezeoekeke.c/ Assignment1
 *
 *Ezeokeke Chiemelie/ CS5600 /Northeastern University
 *
 *Spring2023/Jan192023
 */

//testing the 3 functions implemented in EzeokekeC.CS5600.LearnC.c file by passing a good argument
void unitTest1(){
	double b = 20.425144;
	double* a = &b;
	printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
	printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
	printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
}

//testing the 3 functions implemented in EzeokekeC.CS5600.LearnC.c file by passing a good argument

void unitTest2(){
    double b =50.00;
    double* a = &b;
    printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
}

//testing the 3 functions implemented in EzeokekeC.CS5600.LearnC.c file by passing a bad argument

void unitTest3(){
    double b =-30.00;
    double* a = &b;
    printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
}

//testing the 3 functions implemented in EzeokekeC.CS5600.LearnC.c file by passing a bad argument

void unitTest4(){
    void* a = NULL;
    printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
}

//testing the 3 functions implemented in EzeokekeC.CS5600.LearnC.c file by passing a bad argument
void unitTest5(){
    char str[100] = "hello"; //array of characters
    void * b = NULL; //this is null pointer to convert string into null and give us a warning of an incorrect input.
    void *c = (void *) &str; //creating c in str address
    c = b; //pouring null pointer into c
    // void* b = &str;
    //void* b = NULL;
    
    // void* a = str;
    // printf("v: %s\n", c);
    
   
    // printf("value: %lf\n", (void*)c);
    printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(c));
    printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(c));
    printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(c));
}

int main(){
    printf("=======================unitTest1=====================\n");
	unitTest1();
	printf("======================unitTest2======================\n");
	unitTest2();
    printf("=======================unitTest3=====================\n");
    unitTest3();
    printf("=========================unitTest4===================\n");
    unitTest4();
    printf("=========================unitTest5===================\n");
    unitTest5();
}
