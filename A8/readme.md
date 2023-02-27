# Thread safe custom malloc

------------------------------------------------------------------------------------------------------------------------
1. Introduction
------------------------------------------------------------------------------------------------------------------------
What is a Thread safe malloc?
The standard malloc implementation is by itself not thread-safe. But in our custom malloc our malloc would create an area where it operates multiple allocation.
Each area has its own lock. When a thread needs to allocate memory, pm_malloc_lock() picks an area, locks it, and allocates memory from it. This is what is meant to make 
a malloc thread safe.

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
