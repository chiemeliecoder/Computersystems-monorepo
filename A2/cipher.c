#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#include "polybius.h"

//
// Created by chiem on 1/23/2023.
//


int main(int argc, char const *argv[])
{
//     int counter;
//     //printf("Program Name Is: %s",argv[0]);
//     if(argc==0)
//         printf("\nNo Extra Command Line Argument Passed Other Than Program Name");
//     if(argc>=1)
//     {
//         printf("\nNumber Of Arguments Passed: %d",argc);
//         printf("\n----Following Are The Command Line Arguments Passed----");
//         for(counter=0; counter<argc; counter++)
//             printf("\nargv[%d]: %s",counter,argv[counter]);
//             printf("\nargv[%d]: %s",counter,argv[counter]);
//     }
    
    char *table;
    char *text1 = "hello world",
            *text2 = "Glad to meet you, polybius cipher.";
    char *ciphertext1, *parsetext1,
            *ciphertext2, *parsetext2;

    printf("Randomly generate a table:\n");
    table = generateTable();
    //printTable(table);

    printf("cipher");
    scanf("-e %s",&text1);
    ciphertext1 = pbEncode(table, text1, WITH_BLANK);
    printf("original %s\n", text1);
    printf("ciphertext %s\n", ciphertext1);

    printf("Decipher according to the table\n");
    parsetext1 = pbDecode(table, ciphertext1);
    printf("result %s\n", parsetext1);

    printf("\ntable Encryption 2\n");
    ciphertext2 = pbEncode(table, text2, NO_BLANK);
    printf("original %s\n", text2);
    printf("ciphertext %s\n", ciphertext2);

    printf("Decipher according to the table\n");
    parsetext2 = pbDecode(table, ciphertext2);
    printf("result %s\n", parsetext2);

    free(table);
    free(ciphertext1);
    free(parsetext1);
    free(ciphertext2);
    free(parsetext2);
    return 0;
}
