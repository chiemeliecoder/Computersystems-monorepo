//To ensure that your heap does not get corrupted when allocations are done by multiple concurrently executing threads in C, you need to implement thread-safe memory allocation. Here are some steps that you can follow:

//Use mutex locks: To ensure that only one thread at a time can access the heap, use mutex locks to synchronize access to the heap. When a thread wants to allocate or deallocate memory, it should acquire the lock first and release it when it is done.

#include <pthread.h>

pthread_mutex_t heap_lock;

void* thread_safe_malloc(size_t size) {
    void* ptr = NULL;
    pthread_mutex_lock(&heap_lock);
    ptr = malloc(size);
    pthread_mutex_unlock(&heap_lock);
    return ptr;
}

void thread_safe_free(void* ptr) {
    pthread_mutex_lock(&heap_lock);
    free(ptr);
    pthread_mutex_unlock(&heap_lock);
}

//Use thread-local storage: To avoid contention on the heap, you can use thread-local storage (TLS) to allocate memory that is specific to each thread. TLS allows each thread to have its own private memory pool that it can use for allocations without interfering with other threads.

#include <pthread.h>

__thread void* tls_heap = NULL;

void* thread_safe_malloc(size_t size) {
    void* ptr = NULL;
    if (!tls_heap) {
        tls_heap = malloc(size);  // allocate a private heap for this thread
    }
    ptr = malloc(size);
    return ptr;
}

void thread_safe_free(void* ptr) {
    free(ptr);
}

//Use memory pools: Another way to avoid contention on the heap is to use memory pools. A memory pool is a pre-allocated block of memory that is divided into fixed-size chunks. Each thread can allocate and deallocate memory from its own memory pool without interfering with other threads.

#include <pthread.h>

#define CHUNK_SIZE 1024

typedef struct {
    void* start;
    size_t size;
    size_t used;
} mem_pool_t;

__thread mem_pool_t tls_pool = {0};

void* thread_safe_malloc(size_t size) {
    void* ptr = NULL;
    if (tls_pool.used + size > tls_pool.size) {
        tls_pool.start = malloc(CHUNK_SIZE);
        tls_pool.size = CHUNK_SIZE;
        tls_pool.used = 0;
    }
    ptr = tls_pool.start + tls_pool.used;
    tls_pool.used += size;
    return ptr;
}

void thread_safe_free(void* ptr) {
    // do nothing, since we are using a memory pool
}

//Implement garbage collection: Garbage collection is a technique used in some programming languages to automatically deallocate memory that is no longer being used. By implementing garbage collection, you can avoid memory leaks and reduce the risk of heap corruption. However, implementing garbage collection in C can be complex and is not always practical.

//Test thoroughly: It is important to thoroughly test your program to ensure that it is thread-safe. Test your program with multiple threads and allocate and deallocate memory in different patterns to ensure that there are no race conditions or deadlocks.

//Implementing thread-safe memory allocation can be complex and time-consuming, but it is essential to ensure that your program is reliable and free from memory corruption issues.
