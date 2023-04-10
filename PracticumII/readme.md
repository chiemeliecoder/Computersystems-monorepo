Practicum II
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
File Transfer Protocol implementation in C
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
The server offers the following operations:

```PUT``` : upload a file to server

```GET``` : download file from server

```MD``` : gets the metadata of a file

```RM``` : creates a new folder

```INFO``` : deletes a file or folder

```EXIT``` : terminates the connection.


-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Instructions
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
```c
$ gcc server.c -o server -lpthread
$ ./server <PORT NUMBER> <ADDRESS NUMBER> please use port number 9765 and address number 127.0.0.1
$ gcc client.c -o client
$ ./client file1.txt PUT you can use any of the server operation above for the second argument(PUT, GET, MD, INFO, EXIT)
```
