#include <stdio.h>
#include <stdlib.h> // for rand
#include <time.h> // for time
#include <string.h> // for memcpy, strlen
#include <ctype.h> // for toupper, tolower

#include "polybius.h"

//
// Created by chiem on 1/23/2023.
//

//struct table5x5{
//    int i;
//    int j;
//    int x;
//    int y;
//    int alph[26];
//    char str[255];
//};

//using the table -- you need to define the table structure.
// The input can be a mix of upper and lower case letter.
// You can either ignore special characters or add them to your table.
// Be sure to comment your code to define what occurs with special characters.
// Complete the signature as necessary.
//const char *sz = "hello";




#define TABLE_WIDTH 5
#define ALPHABET {"ABCDEFGHIKLMNOPQRSTUVWXYZ"}

enum TEXT_TYPE { WITH_BLANK, NO_BLANK };

/*
 * create table
 */
char * generateTable()
{
    int i, j, r;
    char tmp[] = ALPHABET;
    char *table = malloc(TABLE_WIDTH * TABLE_WIDTH);

    for (i = 0; i < TABLE_WIDTH; i++)
    {
        for (j = 0; j < TABLE_WIDTH; j++)
        {
            while (1)
            {
                r = randomInt(0, 25);
                if (tmp[r] != 0)
                {
                    table[i * TABLE_WIDTH + j] = tmp[r];
                    tmp[r] = 0;
                    break;
                }
            }
        }
    }

    return table;
}


char * pbEncode(char *table, const char *plaintext, enum TEXT_TYPE type)
{
    char *ciphertext, *format;
    int *arr, i, index;
    int count = 0, len = strlen(plaintext);


    arr = (int *)malloc(len * sizeof(int));
    memset(arr, 0, len * sizeof(int));

    for (i = 0; i < len; i++)
    {

        index = getTableIndex(toupper(plaintext[i]), table);


        if (index)
        {
            arr[count++] = index;
        }
    }


    ciphertext = (char *)malloc(count * 3 + 1);
    memset(ciphertext, 0, count * 3 + 1);

    if (type == WITH_BLANK)
    {
        format = "%s%-3d";
    } else {
        format = "%s%d";
    }

    for ( i = 0; count--; i++)
    {
        sprintf(ciphertext, format, ciphertext, arr[i]);
    }

    return ciphertext;
}

/*
 * decrytion
 */
char * pbDecode(char *table, const char *ciphertext)
{
    char *result, *tmpstr, tmpch, *p;
    int len = strlen(ciphertext), count = 0;
    int i, num;


    tmpstr = (char *)malloc(len + 1);
    strcpy(tmpstr, ciphertext);


    result = (char *)malloc(len / 3 + 1);
    memset(result, 0, len / 3 + 1);

    for (i = 0; i < (len - 1); i++) {
        num = 0;
        p = tmpstr + i;

        if (isdigit(tmpstr[i]) && isdigit(tmpstr[i + 1])) {
            tmpch = tmpstr[i + 2];
            tmpstr[i + 2] = '\0';
            sscanf(p, "%2d", &num);
            tmpstr[i + 2] = tmpch;
            i++;
        }

        if (num > 10)
        {
            result[count++] = getTableChar(num, table);
        }
    }

    return result;
}

static int randomInt(int min, int max)
{
    static int r = 0;
    srand(time(NULL) + r);
    r = rand();
    return (r % max + min);
}

/*
 * get table index
 */
static int getTableIndex(char a, char *table)
{
    int i, j;

    for (i = 0; i < TABLE_WIDTH; i++)
    {
        for (j = 0; j < TABLE_WIDTH; j++)
        {
            if (a == table[i * TABLE_WIDTH + j])
            {
                return (i + 1) * 10 + (j + 1);
            }
        }
    }
    return 0;
}

/*
 * get table
 */
static char getTableChar(int index, char *table)
{
    int i = index / 10 - 1,
            j = index % 10 - 1;
    return tolower(table[i * TABLE_WIDTH + j]);
}

/*
 * print table
 */
static void printTable(char *table)
{
    int i, j;

    for (i = 0; i <= TABLE_WIDTH; i++)
        if (i)
            printf("%d ", i);
        else
            printf("  ");

    printf("\n");

    for (i = 0; i < TABLE_WIDTH; i++)
    {
        printf("%d ", i + 1);
        for (j = 0; j < TABLE_WIDTH; j++)
        {
            printf("%c ", table[i * TABLE_WIDTH + j]);
        }
        printf("\n");
    }

    printf("\n");
}


