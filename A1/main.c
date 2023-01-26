#include <stdio.h>
#include <stdlib.h>
#include "EzeokekeC.CS5600.LearnC.h"

/*Ezeoekeke.c/ Assignment1
 *
 *Ezeokeke Chiemelie/ CS5600 /Northeastern University
 *
 *Spring2023/Jan192023
 */

void unitTest1(){
	double b = 20.425144;
	double* a = &b;
	printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
	printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
	printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
}

void unitTest2(){
    double b =50.00;
    double* a = &b;
    printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
}

void unitTest3(){
    double b =-30.00;
    double* a = &b;
    printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
}

void unitTest4(){
    void* a = NULL;
    printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
}

void unitTest5(){
    char str[100] = "hello";
    char* b = &str;
    double* a = &b;
    printf("value: %f\n", a);
    printf("miles per gallon to kilometer per litre: %lf\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %lf\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %lf\n",kml2mpg(a));
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
