#include <stdio.h>
#include <stdlib.h>
#include "EzeokekeC.CS5600.LearnC.h"

void unitTest1(){
    int a = 20;
    printf("miles per gallon to kilometer per litre: %d\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %d\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %d\n",kml2mpg(a));
}

void unitTest2(){
    int a =50;
    printf("miles per gallon to kilometer per litre: %d\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %d\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %d\n",kml2mpg(a));
}

void unitTest3(){
    int a =-30;
    printf("miles per gallon to kilometer per litre: %d\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %d\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %d\n",kml2mpg(a));
}

void unitTest4(){
    int a = NULL;
    printf("miles per gallon to kilometer per litre: %d\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %d\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %d\n",kml2mpg(a));
}

void unitTest5(){
    int a = "hello";
    printf("miles per gallon to kilometer per litre: %d\n",mpg2kml(a));
    printf("miles per gallon to litre per 100 kilometers: %d\n",mpg2lphm(a));
    printf("kilometer per liter to miles per gallon: %d\n",kml2mpg(a));
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
