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
    char *text1[1000]; //array to store string
    char *text2[1000];
//     char *text1 = "hello world",
//             *text2 = "Glad to meet you, polybius cipher.";
    char *ciphertext1, *parsetext1,
            *ciphertext2, *parsetext2; //pass the ciphered text and  to decipher the text that is a = 44 ciphered text is 44 and parsetext becomes a

    //table generated
    printf("Generate a table:\n");
    table = generateTable();
    //printTable(table);

    //text is ciphered 
    //please write in the command lined after cipher is printed out -e the the two text i.e cipher-e hello world
    //the ciphered text would be printed out
    printf("cipher");
    scanf("-e %s %s",&text1, &text2);
    ciphertext1 = pbEncode(table, text1, NO_BLANK);
    ciphertext2 = pbEncode(table, text2, NO_BLANK);
    printf("original %s\n", text1);
    printf("ciphertext %s\n", ciphertext1);
    printf("\ncipher-e\n");
    printf("original %s\n", text2);
    printf("ciphertext %s\n", ciphertext2);
    
    

    //turning the ciphered text back to its original state is done below
    
    printf("\nDecipher-d\n");
    parsetext1 = pbDecode(table, ciphertext1);
    printf("result %s\n", parsetext1);
    
    printf("\nDecipher-d\n");
    parsetext2 = pbDecode(table, ciphertext2);
    printf("result %s\n", parsetext2);

    free(table);
    free(ciphertext1);
    free(parsetext1);
    free(ciphertext2);
    free(parsetext2);
    return 0;
}
