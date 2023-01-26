#include <stdio.h>
#include <stdlib.h>
#include "EzeokekeC.CS5600.LearnC.h"

/*Ezeoekeke.c/ Assignment1
 *
 *Ezeokeke Chiemelie/ CS5600 /Northeastern University
 *
 *Spring2023/Jan192023
 */

double mpg2kml(double* mpg){
    char *str = (char *) &mpg;
    double c = 0.425144;
    double* convert = &c;
    if(mpg < 0 || mpg == NULL || (void * )mpg == (void * )str ){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }
    
	double kpl;
	kpl = ((*mpg) * (*convert))/1;

	return kpl;
}

double mpg2lphm(double* mpg){
    char *str = (char *) &mpg;
    double c = 235.215;
    double* convert = &c;
    if(mpg < 0 || mpg == NULL || (void * )mpg == (void * )str ){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }
	double lpk;
	lpk = (*convert)/(*mpg);
	return lpk;
}

double kml2mpg(double* kpl){
    char *str = (char *) &kpl;
    double c = 2.35215;
    double* convert = &c;
    if(kpl < 0 || kpl == NULL || (void * )kpl == (void * )str){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }
	double mpg;
	
	mpg = ((*kpl) * (*convert))/1;
	
	return mpg;
}
