#include <stdio.h>
#include <stdlib.h>
#include "EzeokekeC.CS5600.LearnC.h"

/*Ezeoekeke.c/ Assignment1
 *
 *Ezeokeke Chiemelie/ CS5600 /Northeastern University
 *
 *Spring2023/Jan192023
 */

//created a function that that takes miles per gallon as an argument and returns the equivalent kilometers per liter.
//results are in doubles

double mpg2kml(double* mpg){
    char *str = (char *) &mpg;
    double c = 0.425144;
    double* convert = &c;
    if(mpg < 0 || mpg == NULL || (void * )mpg == (void * )str ){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }
    
	double kpl; // kilometers per liter
	kpl = ((*mpg) * (*convert))/1;

	return kpl;
}

//created a function that takes miles per gallon as an argument and returns the equivalent number of liter per 100 kilometers.
//results are in doubles

double mpg2lphm(double* mpg){
    char *str = (char *) &mpg;
    double c = 235.215;
    double* convert = &c;
    if(mpg < 0 || mpg == NULL || (void * )mpg == (void * )str ){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }
	double lpk; //liter per 100 kilometers
	lpk = (*convert)/(*mpg);
	return lpk;
}

//created a function that takes the kilometers per liter and returns miles per gallon
//results are in doubles
double kml2mpg(double* kpl){
    char *str = (char *) &kpl;
    double c = 2.35215;
    double* convert = &c;
    if(kpl < 0 || kpl == NULL || (void * )kpl == (void * )str){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }
	double mpg; //miles per gallon
	
	mpg = ((*kpl) * (*convert))/1;
	
	return mpg;
}
