#include <stdlib.h>

#ifndef POLYBIUS_H
#define POLYBIUS_H

#endif

//
// Created by chiem on 1/23/2023.
//


static int  randomInt(int min, int max);
static void printTable(char *table);
static int  getTableIndex(char a, char *table);
static char getTableChar(int index, char *table);

char * generateTable();

char * encrypt(char *table, const char *plaintext, enum TEXT_TYPE type);

char * decrypt(char *table, const char *ciphertext);
