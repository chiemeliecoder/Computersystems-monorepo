#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define COUNT_TO 30
#define MAX_CORES 2
#define numfiles 4

#define LSIZ 128 
#define RSIZ 100

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
    char line[RSIZ][LSIZ];
	char fname[20];
    FILE *fptr = NULL; 
    int i = 0;
    int tot = 0;
    printf("\n\n Read the file and store the lines into an array :\n");
	printf("------------------------------------------------------\n"); 
	printf(" Input the filename to be opened : ");
	scanf("%s",fname);	

    fptr = fopen(fname, "a");
    while(fgets(line[i], LSIZ, fptr)) 
	{
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    tot = i;
	printf("\n The content of the file %s  are : \n",fname);    
    for(i = 0; i < tot; ++i)
    {
        printf(" %s\n", line[i]);
    }
    printf("\n");
    
    sleep(1);
    
    pthread_mutex_unlock(&mutex);
    
	
    
    //int i = 0;

    FILE *out[numfiles];
    
    for(int k =0; k < numfiles; k++){
        
        //int i = 0;
        
        char filename[100];
        
        sprintf(filename, "results%d.dat", k);
        
        out[k] = fopen(filename,"a");
	
	fputs(line[i],out[k]);
        
        fputs("On the last day of love my heart cracked inside my body.\n",out[k]);
        
        fputs("I spent the entire night casting spells to bring you back.\n", out[k]);

//         fputs("I reached for the last bouquet of flowers you gave me.\n", out[k]);
        
//         fputs("Now wilting in their vase one by one.\n", out[k]);
        
//         fputs("I popped their heads off and ate them.\n", out[k]); 
        
//         fputs("I stuffed a towel at the foot of every door leave.\n", out[k]);
        
//         fputs("I told the air i have no use for you.\n", out[k]);
        
//         fputs("I drew every curtain in the house.\n", out[k]); 
        
//         fputs("Go I told the light no one is coming in and no one is going out.\n", out[k]); 
        
//         fputs("you left and I wanted you still yet.\n", out[k]);
        
//         fputs("I deserved someone who was willing to stay?\n", out[k]);
        
//         fputs("I spend days in bed debilitated by loss.\n", out[k]); 
        
//         fputs("I attempt to cry you back but the water is done and still you have not returned?\n", out[k]); 
        
//         fputs("I pinch my belly till it bleeds.\n", out[k]); 
        
//         fputs("I have lost count of the days sun becomes moon and moon becomes sun.\n", out[k]);
        
//         fputs("I become a ghost!\n" , out[k]);
        
//         fputs("A dozen different thoughts tear through me each second.\n", out[k]); 
        
//         fputs("You must be on your way perhaps.\n" , out[k]);
        
//         fputs("It’s best if you’re not!\n", out[k]); 
        
//         fputs("I am okay.\n" , out[k]);
        
//         fputs("no, I am angry.\n", out[k]); 
        
//         fputs("Yes, I hate you.\n" , out[k]);
        
//         fputs("Maybe I can’t move on.\n" , out[k]);
        
//         fputs("I will forgive you.\n" , out[k]);
        
//         fputs("I want to rip my hair out over and over and over again till my mind exhausts itself into a silence.\n" , out[k]);
        
//         fputs("Yesterday the rain tried to imitate my hands by running down your body,\n", out[k]);
        
//         fputs("I ripped the sky apart for allowing it,\n", out[k]);
        
//         fputs("The hummingbirds tell me you’ve changed your hair\n" , out[k]);
        
//         fputs("I tell them I don’t care while listening to them describe every detai\n", out[k]);
        
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
	sleep(1);

        pthread_mutex_unlock(&mutex);
	char *args[]={"./cipher","-e",line[i],NULL};
        execv(args[0],args);
	
    
	
       

        
        
    }
    
    

   return 0;
}
