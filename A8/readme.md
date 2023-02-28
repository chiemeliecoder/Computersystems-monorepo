# Thread safe custom malloc

------------------------------------------------------------------------------------------------------------------------
# 1.  Introduction
------------------------------------------------------------------------------------------------------------------------
What is a Thread safe malloc?
The standard malloc implementation is by itself not thread-safe. But in our custom malloc our malloc would create an area where it operates multiple allocation.
Each area has its own lock. When a thread needs to allocate memory, pm_malloc_lock() picks an area, locks it, and allocates memory from it. This is what is meant to make a malloc thread safe.

# An issue
When malloc uses a lock to make the threads "take turns" accessing malloc's internal structures. Still, taking turns means one thread is doing nothing for a while, which can't be good for performance, and these locks themselves can take a significant amount of time as they often need to synchronize access across CPU caches and sometimes across physical CPUs.
To solve this issue we need an arena which efficiently handles multi-threaded applications allows for more than one region of memory to be active at a time.


# Below is an example of a thread safe malloc

```c
mstate ar_ptr;
  void *victim;

  arena_lookup(ar_ptr);
  arena_lock(ar_ptr, bytes);
  if(!ar_ptr)
    return 0;
  victim = _int_malloc(ar_ptr, bytes);
  if(!victim) {
    /* Maybe the failure is due to running out of mmapped areas. */
    if(ar_ptr != &main_arena) {
      (void)mutex_unlock(&ar_ptr->mutex);
      ar_ptr = &main_arena;
      (void)mutex_lock(&ar_ptr->mutex);
      victim = _int_malloc(ar_ptr, bytes);
      (void)mutex_unlock(&ar_ptr->mutex);
    } else {
      /* ... or sbrk() has failed and there is still a chance to mmap() */
      ar_ptr = arena_get2(ar_ptr->next ? ar_ptr : 0, bytes);
      (void)mutex_unlock(&main_arena.mutex);
      if(ar_ptr) {
        victim = _int_malloc(ar_ptr, bytes);
        (void)mutex_unlock(&ar_ptr->mutex);
      }
    }
  } else
    (void)mutex_unlock(&ar_ptr->mutex);

  return victim;
```

What is an Arena?

*A structure that is shared among one or more threads which contains references to one or more heaps, as well as linked lists of chunks within those heaps which are "free". Threads assigned to each arena will allocate memory from that arena's free lists.*

------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 2.  Heap Corruption
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
The common forms of corruption are handled with calls to assert which was used in the main.

-------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 3. To improve the performance of Malloc
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
We need to try a few things like:
1. Getting to know how many threads will be making allocations, you can set MALLOC_ARENA_MAX (or, you can call mallopt() in your allocation) to set the number of arenas that make sense for your threads.
2. Try setting the M_MMAP_THRESHOLD environment variable to just under the "very large" size. Allocations larger than this size go directly to mmap(), which means when you free() it, it won't end up in the middle of your heap and prevent coalescing of smaller chunks later.
3. For small allocations, changes to the MALLOC_TRIM_THRESHOLD environment variable might help keep your heap from being too fragmented.
4. The more consistent your allocation sizes are, the easier it is for malloc to find chunks to give you. However, malloc rounds up sizes internally to the size of two pointers (i.e. 8 bytes for 32-bit, or 16 bytes for 64-bit).
5. if your allocations tend to be just a bit bigger than the internal alignment, you're going to be wasting a lot of address space. I.e. don't make a lot of 17-byte requests, because you'll waste 15 bytes per, or nearly half your memory

--------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 4.  Issues and Bugs
--------------------------------------------------------------------------------------------------------------------------------------------------------------------
To improve the performance of my malloc I wasnt able to create large and small allocations functions so as to set the M_MMAP_THRESHOLD and make changes to the MALLOC_TRIM_THRESHOLD variables. Also due to poor memory the thread 2 could can create an exit 1 or segmentation fault due to poor memeory management

--------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 5. How to run
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Please use make to run file it would give you a pm_thread that would give you both threads running.
