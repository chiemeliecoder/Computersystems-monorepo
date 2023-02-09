#include <stdlib.h>

#ifndef POLYBIUS_H
#define POLYBIUS_H

#endif

//
// Created by chiem on 1/23/2023.
//

enum TEXT_TYPE { WITH_BLANK, NO_BLANK };


static int  randomInt(int min, int max);
static void printTable(char *table);
static int  getTableIndex(char a, char *table);
static char getTableChar(int index, char *table);

//generate the table
char * generateTable();

//encoding the inputed word
char * pbEncode(char *table, const char *plaintext, enum TEXT_TYPE type);

//decoding the word encoded
char *  pbDecode(char *table, const char *ciphertext);
