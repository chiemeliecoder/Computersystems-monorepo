**Synchronization with Monitors**

What is a monitor?

a monitor is a synchronization construct that allows threads to have both mutual exclusion and 
the ability to wait (block) for a certain condition to become false. 

*The code is similar to the Producer-Consumer problem.*

We have a bounded buffer of size N

The deposit function writes data into the buffer

The fetch function reads data from the buffer

The idea here is that we have more than one threads that are depositing data
and storing it in a buffer. Meanwhile, one or more fetching threads are fetching the data.

We need to make sure that one thread is accessing the buffer at a time
so we dont produce more data than the buffer can hold.

monitor provides mutually exclusion. 

We are using a circular buffer to avoid which function comes in first.
we don't want to fetch what we don't have.

The monitor deposit function first locks the mutex then checks if the buffer is full.
if it is full we wait till its not full. If the buffer in the deposit function isnt full we would add an item to the buffer
which we did with the for loop inside the deposit then increase the number of items deposited then signals the notempty in the fetch function, so it would unblock the thread then unlock the mutex.

The monitor fetched function first locks the mutex then checks if count is equal to 0 if so we will wait for it to not be empty then we store item from buffer called arr to a variable called res.
increase front position then reduce count of items that was fetched.
If the buffer is full it would signal any blocked threads that is stuck in the while loop in deposit function return stored item then unlock the mutex.

Data is added to the buffer called arr at the rear end then the deposited number increases by 1 then data is fetched from the buffer then stored in the 
res variable then the number of items is then reduced that is how the two functions work mutually exclusively.

There are 4 threads been ran we start with the first with a deposit in the first thread as we have nothing to fetch. We then fetch first then deposit in the next 3 threads.

This is the path to compile the code and run it.

YOUR PATH\src\main\java>javac boundedBuffer.java

YOUR PATH\src\main\java>java boundedBuffer.java

**What the solution looks like it changes as different threads take control of the resources first:**

Thread-0 Current Thread ID: 13 Random number:89753
Thread-0 Current Thread ID: 13 Random number:59969
Before Deposit-0
Data added to rear end--> hello world angus dei
Deposit deposited-1
The number item deposited has increased:1

Data fetched-1
Data in front of the buffer--> hello world angus dei
As we remove the data from the buffer the number of items fetched has now reduced to: 0

Thread-3 Current Thread ID: 16 Random number:90910
Thread-3 Current Thread ID: 16 Random number:99231
Before Deposit-0
Data added to rear end--> hello world angus dei
Deposit deposited-1
The number item deposited has increased:1

Data fetched-1
Data in front of the buffer--> hello world angus dei
As we remove the data from the buffer the number of items fetched has now reduced to: 0

Thread-1 Current Thread ID: 14 Random number:55831
Thread-1 Current Thread ID: 14 Random number:39757
Before Deposit-0
Data added to rear end--> hello world angus dei
Deposit deposited-1
The number item deposited has increased:1

Data fetched-1
Data in front of the buffer--> hello world angus dei
As we remove the data from the buffer the number of items fetched has now reduced to: 0

Thread-2 Current Thread ID: 15 Random number:67795
Thread-2 Current Thread ID: 15 Random number:87903
Before Deposit-0
Data added to rear end--> hello world angus dei
Deposit deposited-1
The number item deposited has increased:1