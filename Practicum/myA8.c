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

typedef struct _Object {
    int refcount;
    // other fields
} Object;

Object* new_object() {
    Object* obj = malloc(sizeof(Object));
    obj->refcount = 1;
    // other initialization code
    return obj;
}

typedef struct _Object {
    int marked;
    struct _Object* next;
    // other fields
} Object;

typedef struct {
    Object* head;
    int size;
} Heap;


Object* new_object(Heap* heap, /* other allocation parameters */) {
    Object* obj = malloc(sizeof(Object));
    obj->marked = 0;
    obj->next = heap->head;
    heap->head = obj;
    heap->size += sizeof(Object);
    // other initialization code
    return obj;
}

void mark(Object* obj) {
    if (obj && !obj->marked) {
        obj->marked = 1;
        // mark any other reachable objects
        mark(obj->field1);
        mark(obj->field2);
        // ...
    }
}

void sweep(Heap* heap) {
    Object** obj = &heap->head;
    while (*obj) {
        if (!(*obj)->marked) {
            // deallocate the object
            Object* unreached = *obj;
            *obj = unreached->next;
            heap->size -= sizeof(Object);
            free(unreached);
        } else {
            // unmark the object for the next GC cycle
            (*obj)->marked = 0;
            obj = &(*obj)->next;
        }
    }
}

void garbage_collect(Heap* heap) {
    mark(root1);
    mark(root2);
    // ...
    sweep(heap);
}


void delete_object(Object* obj) {
    obj->refcount--;
    if (obj->refcount == 0) {
        // deallocate the object
        free(obj);
    }
}

void add_reference(Object* obj) {
    obj->refcount++;
}

int main() {
    // Initialize the heap
    Heap heap;
    heap.head = NULL;
    heap.size = 0;

    // Allocate some objects
    Object* obj1 = new_object(&heap);
    Object* obj2 = new_object(&heap);
    obj1->field1 = obj2;
    obj2->field1 = obj1;

    // Perform a garbage collection cycle
    garbage_collect(&heap);

    // Allocate some more objects
    Object* obj3 = new_object(&heap);
    Object* obj4 = new_object(&heap);
    obj3->field1 = obj4;
    obj4->field1 = obj3;

    // Perform another garbage collection cycle
    garbage_collect(&heap);

    // Free the heap
    sweep(&heap);

    return 0;
}



//Test thoroughly: It is important to thoroughly test your program to ensure that it is thread-safe. Test your program with multiple threads and allocate and deallocate memory in different patterns to ensure that there are no race conditions or deadlocks.

//Implementing thread-safe memory allocation can be complex and time-consuming, but it is essential to ensure that your program is reliable and free from memory corruption issues.


#include <pthread.h>
#include <stdio.h>

// Function executed by each thread
void *thread_function(void *arg) {
    int thread_id = *(int *)arg;
    printf("Thread %d is running\n", thread_id);
    // Other thread code here
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[5];
    int thread_args[5];
    int i;

    // Create threads
    for (i = 0; i < 5; i++) {
        thread_args[i] = i;
        if (pthread_create(&threads[i], NULL, thread_function, &thread_args[i])) {
            fprintf(stderr, "Error creating thread %d\n", i);
            return 1;
        }
    }

    // Wait for threads to finish
    for (i = 0; i < 5; i++) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread %d\n", i);
            return 1;
        }
    }

    return 0;
}

//In this example, the pthread_create function is used to create five threads, each of which executes the thread_function function. The thread_args array is used to pass arguments to the thread functions. Each thread is identified by a unique integer thread ID.

//The pthread_join function is used to wait for the threads to finish executing. It takes the thread ID as its argument and returns when the thread has completed its execution.

//Note that in this example, the threads are executed sequentially, meaning that one thread has to complete its execution before the next thread starts executing. To run threads concurrently, you would need to use synchronization techniques such as mutexes and condition variables.


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define HEAP_SIZE 10000

typedef struct _object {
    struct _object* next;
    int marked;
} Object;

typedef struct _heap {
    Object* head;
    int size;
    pthread_mutex_t lock;
} Heap;

Heap heap;

void* gc_thread(void* args) {
    // Wait for the main thread to signal that garbage collection is needed
    pthread_mutex_lock(&heap.lock);
    pthread_mutex_unlock(&heap.lock);

    // Mark all objects in the heap
    Object* obj = heap.head;
    while (obj != NULL) {
        obj->marked = 1;
        obj = obj->next;
    }

    // Free all unmarked objects
    obj = heap.head;
    Object* prev = NULL;
    while (obj != NULL) {
        if (!obj->marked) {
            if (prev == NULL) {
                heap.head = obj->next;
            } else {
                prev->next = obj->next;
            }
            free(obj);
            heap.size--;
        } else {
            obj->marked = 0;
            prev = obj;
        }
        obj = obj->next;
    }

    // Signal that garbage collection is complete
    pthread_mutex_unlock(&heap.lock);

    return NULL;
}

Object* new_object() {
    Object* obj = malloc(sizeof(Object));
    if (obj == NULL) {
        // Handle allocation failure
        exit(1);
    }
    obj->next = heap.head;
    obj->marked = 0;
    heap.head = obj;
    heap.size++;
    return obj;
}

void* allocate(size_t size) {
    pthread_mutex_lock(&heap.lock);
    void* p = malloc(size);
    pthread_mutex_unlock(&heap.lock);
    return p;
}

void collect_garbage() {
    // Signal the garbage collection thread to start
    pthread_mutex_unlock(&heap.lock);

    // Wait for the garbage collection thread to complete
    pthread_mutex_lock(&heap.lock);
}

int main() {
    // Initialize the heap
    heap.head = NULL;
    heap.size = 0;
    pthread_mutex_init(&heap.lock, NULL);

    // Start the garbage collection thread
    pthread_t gc_thread_id;
    pthread_create(&gc_thread_id, NULL, gc_thread, NULL);

    // Allocate some objects
    Object* obj1 = new_object();
    Object* obj2 = new_object();
    obj1->next = obj2;

    // Perform garbage collection
    collect_garbage();

    // Allocate some more objects
    Object* obj3 = new_object();
    Object* obj4 = new_object();
    obj3->next = obj4;

    // Perform garbage collection
    collect_garbage();

    // Free the heap
    Object* obj = heap.head;
    while (obj != NULL) {
        Object* next = obj->next;
        free(obj);
        obj = next;
    }
    pthread_mutex_destroy(&heap.lock);

    return 0;
}
