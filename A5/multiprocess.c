#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "polybius.c"

#define COUNT_TO 30
#define MAX_CORES 2
#define numfiles 4 //number of files

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long long i = 0;


void *start_counting(FILE *out) 
{
    //lock
    pthread_mutex_lock(&mutex);

    while (i < COUNT_TO) 
    {
        ++i;

        printf("i = %lld\n", i);
        fputs("Inside Sum function...\n", out);
        fprintf(out,"i = %lld\n", i);
    }

    //lock
    pthread_mutex_unlock(&mutex);
}


int main(void) 
{
    int i = 0;

    FILE *out[numfiles];
    
    for(int k =0; k < numfiles; k++){
        
        int i = 0;
        
        char filename[100];
        
        sprintf(filename, "results%d.dat", k); //change the file name by counting up getting result0, result 1, result 3 file. run code and check your directory for the new files with the information
        
        out[k] = fopen(filename,"a"); //puts the sentences in the files 
        
        //the 30 sentences below
        
        fputs("On the last day of love my heart cracked inside my body.\n",out[k]);
        
        fputs("I spent the entire night casting spells to bring you back.\n", out[k]);

        fputs("I reached for the last bouquet of flowers you gave me.\n", out[k]);
        
        fputs("Now wilting in their vase one by one.\n", out[k]);
        
        fputs("I popped their heads off and ate them.\n", out[k]); 
        
        fputs("I stuffed a towel at the foot of every door leave.\n", out[k]);
        
        fputs("I told the air i have no use for you.\n", out[k]);
        
        fputs("I drew every curtain in the house.\n", out[k]); 
        
        fputs("Go I told the light no one is coming in and no one is going out.\n", out[k]); 
        
        fputs("you left and I wanted you still yet.\n", out[k]);
        
        fputs("I deserved someone who was willing to stay?\n", out[k]);
        
        fputs("I spend days in bed debilitated by loss.\n", out[k]); 
        
        fputs("I attempt to cry you back but the water is done and still you have not returned?\n", out[k]); 
        
        fputs("I pinch my belly till it bleeds.\n", out[k]); 
        
        fputs("I have lost count of the days sun becomes moon and moon becomes sun.\n", out[k]);
        
        fputs("I become a ghost!\n" , out[k]);
        
        fputs("A dozen different thoughts tear through me each second.\n", out[k]); 
        
        fputs("You must be on your way perhaps.\n" , out[k]);
        
        fputs("It’s best if you’re not!\n", out[k]); 
        
        fputs("I am okay.\n" , out[k]);
        
        fputs("no, I am angry.\n", out[k]); 
        
        fputs("Yes, I hate you.\n" , out[k]);
        
        fputs("Maybe I can’t move on.\n" , out[k]);
        
        fputs("I will forgive you.\n" , out[k]);
        
        fputs("I want to rip my hair out over and over and over again till my mind exhausts itself into a silence.\n" , out[k]);
        
        fputs("Yesterday the rain tried to imitate my hands by running down your body,\n", out[k]);
        
        fputs("I ripped the sky apart for allowing it,\n", out[k]);
        
        fputs("The hummingbirds tell me you’ve changed your hair\n" , out[k]);
        
        fputs("I tell them I don’t care while listening to them describe every detai\n", out[k]);
        
        fputs("I envy the winds who still witness you\n", out[k]);
        
        // create a thread group the size of MAX_CORES
        pthread_t *thread_group = malloc(sizeof(pthread_t) * MAX_CORES);
    
        // start all threads to begin work
        for (i = 0; i < MAX_CORES; ++i) 
        {
            pthread_create(&thread_group[i], NULL, start_counting, out[k]);
            
        }
    
        // wait for all threads to finish
        for (i = 0; i < MAX_CORES; ++i) 
        {
            pthread_join(thread_group[i], NULL);
        }
        fclose(out[k]);
        
    }
    sleep(1);
    
    pthread_mutex_unlock(&mutex);
    
    char *filename = "Multiprocessing.txt";
    FILE *fp = fopen(filename, "r");
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
    printf("\ncipher");
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

    printf(" File %s successfully encrypted ..!!\n\n", fp); 
    fclose(fp);
    
    
    
    
    // // create a thread group the size of MAX_CORES
    // pthread_t *thread_group = malloc(sizeof(pthread_t) * MAX_CORES);

    // // start all threads to begin work
    // for (i = 0; i < MAX_CORES; ++i) 
    // {
    //     pthread_create(&thread_group[i], NULL, start_counting, out[numfiles]);
        
    // }

    // // wait for all threads to finish
    // for (i = 0; i < MAX_CORES; ++i) 
    // {
    //     pthread_join(thread_group[i], NULL);
    // }

    // fclose(out);

    return EXIT_SUCCESS;
}




// #include <stdio.h>  
// #include <stdlib.h> 
// #include <sys/types.h> 
// #include <unistd.h>  

// //#include <process.h>

// void main() { 	 

// char fname[40], ch; 	 

// FILE *fpts, *fptt; 	 	 

// printf("\n\n Encrypt a text file :\n"); 	 

// printf("--------------------------\n"); 	 	 	 

// printf(" Input the name of file to encrypt : "); 	 

// scanf("%s",fname);	

// //execlp("cipher",(char *) 0);

// fpts=fopen(fname, "r"); 	 

// if(fpts==NULL) 	{ 		 

// printf(" File does not exists or error in opening..!!"); 		 

// exit(1); 	} 	 

// fptt=fopen("Multiprocessing.txt", "w"); 	 

// if(fptt==NULL) 	{ 		 

// printf(" Error in creation of file temp.txt ..!!"); 		 

// fclose(fpts); 		
// exit(2); 	
    
// } 	
// while(1){
//     ch=fgetc(fpts); 		
//     if(ch==EOF){ 			
//         break;
        
//     }else{ 			
//         ch=ch+100; 			
//         fputc(ch, fptt); 		
        
//     } 	
    
// } 	
// fclose(fpts); 	
// fclose(fptt); 	
// fpts=fopen(fname, "w"); 	
// if(fpts==NULL){ 		
//     printf(" File does not exists or error in opening..!!"); 		
//     exit(3); 	
    
// } 	
//     fptt=fopen("Multiprocessing.txt", "r"); 	
//     if(fptt==NULL){
//         printf(" File does not exists or error in opening..!!"); 		
//         fclose(fpts); 		
//         exit(4); 	
        
//     } 	while(1){ 		
//         ch=fgetc(fptt); 		
//         if(ch==EOF){ 			
//             break; 		
            
//         } else{ 			
//             fputc(ch, fpts); 		
            
//         } 	
        
//     } 	
//     printf(" File %s successfully encrypted ..!!\n\n", fname); 	
//     fclose(fpts); 	
//     fclose(fptt); 
    
// } 
