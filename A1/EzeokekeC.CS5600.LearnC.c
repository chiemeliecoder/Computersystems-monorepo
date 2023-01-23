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
	int kpl;
	kpl = (mpg * 0.425144)/1;
	return kpl;
}

int mpg2lphm(int mpg){
	int lpk;
	lpk = (235.215)/mpg;
	return lpk;
}

int kml2mpg(int kpl){
	int mpg;
	mpg = (kpl * 2.35215)/1;
	return mpg;
}


