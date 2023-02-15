import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.Random;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.*;
import java.util.*;

/**
 * A bounded buffer. It's a cyclic buffer with a certain capacity with a start and an end.
 * You can make the current thread wait on the condition using the await method (and its variant with timeout)
 * and you can signal threads using signal and signalAll methods*/

/**
 * this is Condition Synchronization
 * */

public class boundedBuffer {
  private final String[] arr; //buffer
  private final int capacity; //size of buffer

  private int front; //position in the buffer in front
  private int rear; //the rear end position in the buffer
  private int count = 0; //number of items in the buffer

  private final Lock lock = new ReentrantLock();

  private final Condition notFull = lock.newCondition();
  private final Condition notEmpty = lock.newCondition();



  /**constructor
   **/
  public boundedBuffer(int capacity) {
    super();
    this.capacity = capacity;
    arr = new String[capacity];

  }

  /**
   * The monitor deposit function first locks the mutex then checks if the buffer is full. if it is full we wait till its not
   * full. If the buffer in the deposit function isnt full we would add an item to the buffer which we did with the
   * for loop inside the deposit then increase the number of items deposited then signals the notempty in the fetch function,
   * so it would unblock the thread then unlock the mutex.*/

  public void deposit(String data) throws InterruptedException {
    lock.lock();


    try {
      while(count == capacity){ //if buffer is full we wait till its not full
        notFull.await();
      }

      //add item to buffer

      for(int i = 0; i < capacity; i++){

        arr[i] = data;
        arr[i] = Thread.currentThread().getName() + " Current Thread ID: " + Thread.currentThread().getId() + " Random number:" + (10000 + ThreadLocalRandom.current().nextInt(89999));

        System.out.println(arr[i%capacity] + "");
        //System.out.println(arr[i]);
      }



      System.out.println("Before Deposit-"
          + count);
      arr[rear] = data;
      System.out.println("Data added to rear end--> " + arr[rear]);

      rear = (rear + 1) % capacity;
      count++;

      System.out.println("Deposit deposited-"
          + count);

      System.out.println("The number item deposited has increased:" + count +"\n");


//      System.out.println(data);
//      System.out.println(rear);
//      System.out.println(count);

      notEmpty.signal(); //signals the notempty in fetch so it would be unblocked

    }finally {
      lock.unlock();
    }
  }

  /**
   * The monitor fetched function first locks the mutex then checks if count is equal to 0 if so we will wait
   * for it to not be empty then we store item from buffer called arr to a variable called res.
   * increase front position then reduce count of items that was fetched.
   * If the buffer is full it would signal any blocked threads that is stuck in the while loop in deposit function
   * return stored item then unlock the mutex.*/

  public String fetch() throws InterruptedException {
    lock.lock();


    try {
      while(count == 0){ //while count is equal to 0 we will wait for it to not be empty
        notEmpty.await();
      }

      System.out.println("Data fetched-"
          + count);
      //store item from buffer to res

      String res= arr[front];

      System.out.println("Data in front of the buffer--> " + res);


      front = (front + 1) % capacity;
      count--;

      System.out.println("As we remove the data from the buffer the number of items fetched has now reduced to: " + count + "\n");


      notFull.signal(); //if the buffer is full this signal would all any blocked threads that is stuck in the while loop in deposit function

      return res; //return stored item

    }finally {
      lock.unlock();
    }


  }
  public static void main(String[] args) throws InterruptedException {
    /**
     * There are 4 threads been ran we start with the first with a deposit in the first thread
     * as we have nothing to fetch. We then fetch first then deposit in the next 3 threads.
     * */
    int cap = 30; //capacity or size of buffer. 30 random number string
    String str = "hello world angus dei"; //string inputed
    boundedBuffer bb = new boundedBuffer(cap); //so we can call the fetch and deposit functions

    final ExecutorService executorService = Executors.newSingleThreadExecutor();

    Thread thread1 = new Thread(new Runnable() { //thread 1 only deposits cant fetch since there is nothing to fetch
      @Override
      public void run() {
        try{

          bb.deposit(str);

        } catch (InterruptedException e) {
          e.printStackTrace();
        }
      }
    });

    Thread thread2 = new Thread(new Runnable() { //fetch from the first thread then deposit
      @Override
      public void run() {
        try{
          bb.fetch();
          bb.deposit(str);

        } catch (InterruptedException e) {
          e.printStackTrace();
        }
      }
    });

    Thread thread3 = new Thread(new Runnable() { //fetch from the second thread then deposit
      @Override
      public void run() {
        try{
          bb.fetch();
          bb.deposit(str);


        } catch (InterruptedException e) {
          e.printStackTrace();
        }
      }
    });

    Thread thread4 = new Thread(new Runnable() { //fetch from the third thread then deposit
      @Override
      public void run() {
        try{
          bb.fetch();
          bb.deposit(str);

        } catch (InterruptedException e) {
          e.printStackTrace();
        }
      }
    });




//    executorService.submit(thread1);
//    executorService.submit(thread2);
//    executorService.submit(thread3);


    //start the thread process or begin the execution of the threads
    thread1.start();
    thread2.start();
    thread3.start();
    thread4.start();


    //Join is a synchronization method that blocks the calling thread (that is, the thread that calls the method)
    // until the thread whose Join method is called has completed.
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();



    //executorService.shutdown();

    //System.out.println("Thread main finished");
  }
}

/**
 * The two methods are protected with the lock to ensure mutual exclusion
 * Then we use two conditions variables. One to wait for the buffer to be not empty and an other one to wait for the buffer to be not full.
 * You can see that I have wrapped the await operation on a while loop. This is to avoid signal stealers problem that can occurs when using Signal & Continue
 * And that BoundedBuffer can be easily used with several threads with no problems.*/