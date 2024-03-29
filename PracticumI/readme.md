Practicum I

-------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 1 introduction and explanation of code
-------------------------------------------------------------------------------------------------------------------------------------------------------------------

The given code covers the requirement to build a program-managed heap that allows programs to allocate memory from that heap. The code defines a struct called block that stores metadata about the allocated blocks of memory, and uses a linked list of block structs to keep track of free memory.

The code also defines a function pm_malloc_lock that allocates memory from the program-managed heap. 

The code includes various header files and defines several constants such as the number of threads, the page size, the number of pages, the allocation size, and the number of mallocs to perform. It also defines a swap file name and pre-allocates a "heap" memory from the static store.

The code declares several structs and typedefs, including a block struct that stores information about memory blocks and a Page struct that stores information about memory pages. There is also a PageTable struct that contains an array of Page structs and a num_pages field to keep track of the number of pages in the table.

The code defines a bitfield struct to store used and dirty status of a page, as well as a union struct that can be used to access the bits in the page_status_t struct. Additionally, the code defines a typedef for access_status_t, which is a boolean value that indicates whether a page has been accessed or not. There is also a typedef for access_tracker_t, which is a bit vector that tracks the access status of pages.

The code includes a mymalloc.h header file and initializes a mutex variable for thread synchronization. The init function initializes the freeList struct and opens the swap file. The frag function is used to split a block of memory into two smaller blocks if the memory is too large for the requested size. The allocate_page function allocates a physical memory page and maps it to a virtual page.

Overall, the code establishes memory management, including allocation and swapping pages in and out of physical memory

--------------------------------------------------------------------------------------------------------------------------------------------------------------------
#3 to handle the swap file
---------------------------------------------------------------------------------------------------------------------------------------------------------------------
My swap file on my desktop shows this 
```c
-rw------- 1 root root 677570560 Mar 21 21:15 /swapfile
```
To handle the swap file
```c
sudo fallocate -l 1G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```
Check if the swap file is mounted by running the following command
```c
swapon -s
```
If the swap file is not listed you can do it manually with 
```c
sudo swapon /swapfile
```

it would show 
```c
Filename				Type		Size	Used	Priority
/swapfile                              	file    	661684	0	-2
```

```c
# /etc/fstab: static file system information.
#
# Use 'blkid' to print the universally unique identifier for a
# device; this may be used with UUID= as a more robust way to name devices
# that works even if disks are added and removed. See fstab(5).
#
# <file system> <mount point>   <type>  <options>       <dump>  <pass>
# / was on /dev/sda5 during installation
UUID=4cf1eff4-dd69-40ce-8f0a-cd53ed08847b /               ext4    errors=remount-ro 0       1
# /boot/efi was on /dev/sda1 during installation
UUID=4B71-F283  /boot/efi       vfat    umask=0077      0       1
/swapfile                                 none            swap    sw              0       0
```

So to work with the swapfile you use this instructions below 
Tp check if the swap file exists: You can use the command `ls -lh /swapfile` to check if the swap file exists. If it does not exist, you can create it using the command `sudo fallocate -l 1G /swapfile`.

Set the correct permissions for the swap file: You can use the command `sudo chmod 600 /swapfile` to set the correct permissions for the swap file. 

Activate the swap file: You can use the command `sudo mkswap /swapfile` to activate the swap file. an error would occur  

This error message indicates that the /swapfile is already being used as a swap space. You can check if it's currently in use by running the command `sudo swapon -s`. 

If it's being used, you'll need to turn off the swap space first by running `sudo swapoff /swapfile`. 

Then, you can run the `sudo mkswap /swapfile` command again to create the swap space. Once the swap space is created, you can turn it back on by running `sudo swapon /swapfile`. 

Enable the swap file: You can use the command `sudo swapon /swapfile` to enable the swap file. 

here are pictures to help with handling swapfiles
![swaponandoff](https://user-images.githubusercontent.com/77821039/226798043-d3112f7b-f782-4df0-9412-a7c65cf04ea6.PNG)

![swappinfile](https://user-images.githubusercontent.com/77821039/226798063-6a3aa244-2a48-46e7-a573-da9827c4fb4a.PNG)

![prevfiledate](https://user-images.githubusercontent.com/77821039/226798022-109ff2c4-1756-452e-be29-f6ac22a824bf.PNG)

![ismounted](https://user-images.githubusercontent.com/77821039/226797999-b2a480c3-63ef-4b6c-91ea-d9e42581b6ab.PNG)

![changeinswapfile date](https://user-images.githubusercontent.com/77821039/226797978-acfbe3e0-745a-4fbe-a42a-442ef3e4198c.PNG)


-------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 3 Tests
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
first test unit test 1 checks memeory allocation that is it allocates memory equal to the size of a single page and read/write data from/to that memory location. 

second test unit test 1 checks memeory allocation that is it allocates more memory than the size of the physical memory and verify that the page is swapped to virtual memory and can still be read/written to. 

fourth test unit test 1 checks memeory  allocation  and free that is it allocates memory and free it, then try to read/write from the freed memory location and ensure that it results in an error. 

The fifth test the page replacement algorithm by allocating more memory than the size of the physical memory and accessing different pages to verify that the least recently used page is swapped to virtual memory. 

I also tested the thread-safety of the program-managed heap by allocating and freeing memory from multiple threads simultaneously. 

a segmentation error would occur in my test 4 as my unit test 4 is used to Allocate memory and free it, then try to read/write from the freed memory location and ensure that it results in an error
