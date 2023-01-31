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
/**
 * Add a short explanation of 100-300 words to your code as a comment.
 * I defined the number of threads
 * created an array that with the size of the number of threads multiplied by 10
 * I created an array index
 * Then created pthread_mutex_t is used to declare an object of type mutex
 * Then created 3 functions one called hello other called worker
 * The hello fucntion prints the thread id and the name of the thread if it is Y or X
 * The worker function locks and unlocks the thread to prevent data race
 * In the main function the runs the functions and prints out the Thread ID, name and lock and unlock the pthread_mutex_t
 **/

#define NOTHREADS 5


int arr[10 * NOTHREADS]; //array
int aindex; //array index

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

/**Question 3.
 * Can a threads "communicate", i.e., can they exchange or share data?
 * Yes they can.Threads are multiple programs running at the same time but sharing variables.
 * Typically, every thread can access all of the application’s memory.
 * Shared variables are the core strength of threads.
 * They allow threads to communicate easily and quickly,
 * but they also make it possible for threads to get in each other’s way.
 * The fundamental problem is that multiple threads were accessing share data at the same time,
 * without proper locking or other synchronization to make sure that one occurred after the other.
 * This situation is called a data race**/

/**Question 4.
 * Do you get consistency?
 * No I didn't at a point in the threading process the thread ID was higher at the point where it was cancelled
 *
 * Why is it consistent?
 * it wasn't consistent
 *
 * Would you expect it to be inconsistent?
 * The program is exiting before the threads are complete.
 * That is why I see different outputs on different runs:
 * The program exits more or less quickly allowing less or more of the work to complete.
 *
 * How would you avoid problems where threads step on each other?
 * There are a number of ways to avoid threads from step on each other.
 * straightforward programming constructs that can avoid multi-threading conflicts;
 * With static variables or objects that do not need to share information between threads,
 * you will need to ensure that different threads access different storage;
 * in this case by using a static array with a separate element for each thread. T
 * The ideal place to initialize such an array would be during data processing (calling a user function from a data view variable or a database view variable),
 * because this is done single-threaded but the number of threads is known.
 * The thread information may be accessed by a user function with its Additional argument property set to Information.
 *
 * With input and output you can protect threads from step on each other
 * against this being done by multiple threads at once by creating a critical section defined as a global variable in your user function source code.
 * This may then be initialized during a single-threaded part of the run,
 * You can use a global boolean variable to ensure that the critical section is initialized only once (the first time the user function is called).
 * If a static object does need to share information between threads then the latter method can be adapted for access to the single object.
 *
 * Provide a short explanation of the phenomena that you see. You do not have to resolve the problem,
 * but only explain what the problem is and what it's resolution might require.
 **/

