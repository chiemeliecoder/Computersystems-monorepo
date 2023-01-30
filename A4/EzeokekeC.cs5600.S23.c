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
void *worker(void *data)
{
    char *name = (char*)data;
    for (int i = 0; i < 120; i++)
    {usleep(50000);
        printf("Hi from thread name = %s\n", name);
        printf(" Thread ID = %d\n ", pthread_self());
    }
    printf("Thread %s done!\n", name);
    return NULL;
}

//int main(void){
//    pthread_t th1, th2;
//    pthread_create(&th1, NULL, worker, "X");
//    pthread_create(&th2, NULL, worker, "Y");
//    sleep(5);
//    printf("Exiting from main program\n");
//    return 0;
//}
//
//int main(void)
//{
//    pthread_t th1, th2;
//    pthread_create(&th1, NULL, worker, "X");
//    pthread_create(&th2, NULL, worker, "Y");
//    sleep(5);
//    printf("exiting from main program\n");
//    pthread_join(th1, NULL);
//    pthread_join(th2, NULL);
//    return 0;
//}

int main(void)
{
    pthread_t th1, th2;
    pthread_create(&th1, NULL, worker, "X");
    pthread_create(&th2, NULL, worker, "Y");
    sleep(1);
    printf("====> Cancelling Thread Y!!%d....\n",th1);
    pthread_cancel(th2);
    usleep(100000);
    printf("====> Cancelling Thread X!%d....\n", th2);
    pthread_cancel(th1);
    printf("exiting from main program\n");
    return 0;
}


