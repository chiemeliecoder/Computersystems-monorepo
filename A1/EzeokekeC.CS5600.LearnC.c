#include <stdio.h>
#include <stdlib.h>
#include "EzeokekeC.CS5600.LearnC.h"

/*Ezeoekeke.c/ Assignment1
 *
 *Ezeokeke Chiemelie/ CS5600 /Northeastern University
 *
 *Spring2023/Jan192023
 */

int mpg2kml(int mpg){
    char str[100];
    if(mpg < 0 || mpg == NULL ){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }

    int kpl;
    kpl = (mpg * 0.425144)/1;
    return kpl;
}

int mpg2lphm(int mpg){
    char str[100];
    if(mpg < 0 || mpg == NULL ){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }
    int lpk;
    lpk = (235.215)/mpg;
    return lpk;
}

int kml2mpg(int kpl){
    char str[100];
    if(kpl < 0 || kpl == NULL || kpl == str){
        printf("Wrong input try numbers greater than 0: ");
        return -1;
    }
    int mpg;
    mpg = (kpl * 2.35215)/1;
    return mpg;
}


