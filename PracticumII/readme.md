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
$ ./fgets file1.txt PUT you can use any of the server operation above for the second argument(PUT, GET, MD, RM, INFO, EXIT)
for the piping we would use
$ ./server | tee file1.txt (he tee command is a Unix and Linux command-line utility used to read standard input and write it to both standard output and one or more files simultaneously.)
$ ./server | ls -l (another program that can be used in this command line. It will list the files in the current directory along with additional information such as file permissions, owner, group, size, and modification time.)
for making a directory you need to use mkdir first then run MD see below
$ mkdir myfolder
$ ./fgets MD TestforPII/myfolder OR /fgets MD YOUR_PATH/myfolder

```
