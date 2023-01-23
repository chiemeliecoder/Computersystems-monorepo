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

int main(){
	unitTest1();
	printf("============================================\n");
	unitTest2();
}
