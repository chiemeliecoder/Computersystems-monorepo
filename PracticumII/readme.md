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
$ ./server <PORT NUMBER> please use port number 8958
$ gcc fgets.c -o fgets
for GET client commandline
$ ./fgets <command> <remote_file_path> <local_file_path>
$ fget GET folder/foo.txt data/localfoo.txt
for INFO client commandline
$ fget INFO folder/foo.txt <local_file_path>
for MD client commandline
$ ./fgets MD folder/newfolder <local_file_path>
for RM client commandline
$ ./fgets RM <remote_folder_path> <local_file_path>
for RM client commandline
$ ./fgets PUT <local_file_path> <remote_path>
for the piping we would use
$ ./fgets INFO remote_file_path local_file_path | wc -l (another program that can be used in this command line. It will list the files in the current directory along with additional information such as file permissions, owner, group, size, and modification time.)


```
