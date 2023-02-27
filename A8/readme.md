# Thread safe custom malloc

------------------------------------------------------------------------------------------------------------------------
1. Introduction
------------------------------------------------------------------------------------------------------------------------
What is a Thread safe malloc?
The standard malloc implementation is by itself not thread-safe. But in our custom malloc our malloc would create an area where it operates multiple allocation.
Each area has its own lock. When a thread needs to allocate memory, pm_malloc_lock() picks an area, locks it, and allocates memory from it. This is what is meant to make a malloc thread safe.

# An issue
When malloc uses a lock to make the threads "take turns" accessing malloc's internal structures. Still, taking turns means one thread is doing nothing for a while, which can't be good for performance, and these locks themselves can take a significant amount of time as they often need to synchronize access across CPU caches and sometimes across physical CPUs.


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

# What is an Arena?
A structure that is shared among one or more threads which contains references to one or more heaps, as well as linked lists of chunks within those heaps which are "free". Threads assigned to each arena will allocate memory from that arena's free lists.
