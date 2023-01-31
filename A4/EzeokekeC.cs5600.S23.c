#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//With the pthread_join function, you can wait for a thread to terminate instead.
// The thread using this function will block until the expected thread terminates
//Sometimes there are situations where joining with pthread_join does not make sense;
//if it’s impossible to predict when the thread will end, use pthread_detach function:
//You can cancel a thread with a call to pthread_cancel,
//The pthread_self() function is used to get the ID of the current thread.

#define NOTHREADS 5

/*
(1) array
(2) array index
*/
int arr[10 * NOTHREADS];
int aindex;

pthread_mutex_t mutex;


void *hello(void *data)
{
    char *name = (char*)data;
    for (int i = 0; i < 120; i++)
    {usleep(50000); //The usleep() function suspends execution of the calling thread for (at least) usec microseconds
        printf("Hi from thread name = %s\n", name);
        printf(" Thread ID = %d\n ", pthread_self());
        //printf(" Thread ID = %d\n ", i);
    }
    printf("Thread %s done!\n", name);
    return NULL;
}

void *worker(void *data)
{

    int i;
    int *id = (int *) data;

    //char *name = (char*)data;
    // for (int i = 0; i < 120; i++){
    //     usleep(50000);
    //     printf("Hi from thread name = %s\n", name);
    //     //printf(" Thread ID = %d\n ", pthread_self());
    //     printf(" Thread ID = %d\n ", i);
    // }
    for (int i = 1; i <= 10; i++){
        // printf("Hi from thread name = %s\n", name);
        // printf("Thread ID = %d\n ", arr[aindex]);

        pthread_mutex_lock(&mutex);
        arr[aindex] = (*id)*100+ i;
        printf("add to thread 1:%d..\n", arr[aindex]);
        sleep(1);
        aindex = aindex + 1;
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        arr[aindex] = (*id)*(i * 2);
        printf("add to thread 2:%d..\n", arr[aindex]);
        sleep(1);
        aindex = aindex + 2;
        pthread_mutex_unlock(&mutex);


    }
    //printf("Thread %s done!\n", name);
    pthread_exit(NULL);
    //return NULL;
}

int main(int argc, char **argv){
    pthread_t tids[NOTHREADS];
    int ids[NOTHREADS] = {1, 2, 3, 4, 5};
    int ret;
    long t;
    int nthreads; //number of threads
    int i;

    pthread_mutex_init(&mutex, NULL);
    pthread_t th1, th2;
    pthread_create(&th1, NULL, hello, "X");
    pthread_create(&th2, NULL, hello, "Y");
    sleep(1);
    printf("====> Cancelling Thread at Y!! %ld....\n", th1);
    pthread_cancel(th2);
    usleep(100000);
    printf("====> Cancelling Thread at X!! %ld....\n", th2);
    pthread_cancel(th1);
    for (i=0 ; i<NOTHREADS; i++) {
        printf("%d %s - Creating thread #%d \n", __LINE__, __FUNCTION__, i);
        ret = pthread_create(&tids[i], NULL, worker, &ids[i]);
        if (ret) {
            printf("unable to create thread! \n");
            exit(-1);
        }
    }

    for (i=0 ; i<NOTHREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    printf("Final array : \n");
    for (i=0; i<50; i++)
        printf("%d ", arr[i]);
    printf("\n\n");
    // Read the number of threads to create from the command line.
    if (argc !=2) {
        fprintf(stderr, "usage: %s <n>\n", argv[0]);
        fprintf(stderr, "where <n> is the number of threads\n");
        return 1;
    }
    nthreads = strtol(argv[1], NULL, 10);




    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    printf("exiting from main program\n");
    return 0;
}



/**Question 4.
 * Do you get consistency?
 * Why is it consistent?
 * Would you expect it to be inconsistent?
 * How would you avoid problems where threads step on each other?
 * Provide a short explanation of the phenomena that you see. You do not have to resolve the problem,
 * but only explain what the problem is and what it's resolution might require.
 **/

