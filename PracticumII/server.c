/*
 * server.c -- TCP Socket Server
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFSIZE 1024


// I used BUFSIZ instead of the BUFFSIZE I defined because It is generally better to use the predefined constant BUFSIZ instead of defining your own buffer size constant BUFFSIZE. This ensures that the buffer size is set to a reasonable value that is suitable for the system on which the program is running. 


/**
This function reads data from a socket connection and writes it to two separate files named file2-1.txt and file2-2.txt. The function receives data in chunks of size BUFSIZ and writes them to the files using the fprintf() function. The function continues to read data until there is no more data to read or an error occurs. Once all the data has been written to the files, the function closes the files and returns.
*/

void write_file(int socket_desc){
  int n;
  FILE *fp1 *fp2;
  char *filename1 = "file2-1.txt";
  char *filename2 = "file2-2.txt";
  char buffer[BUFSIZ];

  fp1 = fopen(filename1, "w");
  fp2 = fopen(filename2, "w");
  
  while (1) {
    // Receive client's message:
    n = recv(socket_desc, buffer, BUFSIZ, 0);
    if (n <= 0){
      break;
      return;
    }
    // Write to first USB drive
    fprintf(fp1, "%s", buffer);
    
    // Write to second USB drive
    fprintf(fp2, "%s", buffer);
    
    bzero(buffer, BUFSIZ);
  }
  fclose(fp1);
  fclose(fp2);
  
  return;
}


/**
This function is_usb_drive takes a file path as an argument and checks whether the specified device is a USB drive or not. It does this by first opening the file, checking if it is a block device and then opening the USB bus to check if the device has a USB interface. Finally, it returns a boolean value indicating whether the device is a USB drive or not.It returns 1 if the specified device path is a block device with a USB interface, and 0 otherwise.
*/

int is_usb_drive(const char* path) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    perror("Error opening device");
    return 0;
  }

  struct stat statbuf;
  if (fstat(fd, &statbuf) < 0) {
    perror("Error getting device stats");
    close(fd);
    return 0;
  }

  if (!S_ISBLK(statbuf.st_mode)) {
    // Not a block device
    close(fd);
    return 0;
  }

  // Check if device has a USB interface
  int has_usb_interface = 0;
  int usb_fd = open("/dev/bus/usb", O_RDONLY);
  if (usb_fd < 0) {
    perror("Error opening USB bus");
  } else {
    struct usbdevfs_getdriver driver;
    driver.interface = -1;
    ioctl(fd, USBDEVFS_GETDRIVER, &driver);
    if (driver.interface >= 0) {
      has_usb_interface = 1;
    }
    close(usb_fd);
  }

  close(fd);

  return has_usb_interface;
}



/**
The identify_usb_drive2() function identifies the path to a USB drive by scanning the "/media" directory and checking if each directory in it corresponds to a USB drive using the is_usb_drive() function. If a USB drive is found, it returns the path to the drive, otherwise, it returns NULL.
*/

char* identify_usb_drive2() {
  struct dirent **namelist;
  int num_drives = scandir("/media", &namelist, 0, alphasort);
  if (num_drives == -1) {
    perror("Error in scanning directory");
    return NULL;
  }

  char *usb_path = NULL;
  for (int i = 0; i < num_drives; i++) {
    if (strcmp(namelist[i]->d_name, ".") != 0 && strcmp(namelist[i]->d_name, "..") != 0) {
      char *full_path = malloc(strlen("/media/") + strlen(namelist[i]->d_name) + 1);
      sprintf(full_path, "/media/%s", namelist[i]->d_name);
      if (is_usb_drive(full_path)) {
        usb_path = full_path;
        break;
      } else {
        free(full_path);
      }
    }
    free(namelist[i]);
  }
  free(namelist);

  return usb_path;
}





/*
The write_to_usb_drive function takes a string of data and the path to a USB drive as input. It opens the USB drive in append mode, writes the data to it using the fprintf function, and then closes the file. If an error occurs while opening the USB drive, it prints an error message and returns without writing anything to the drive.
*/


void write_to_usb_drive(const char* data, const char* path) {
  FILE* fp = fopen(path, "a"); // open the USB drive in append mode
  if (fp == NULL) {
    perror("Error opening USB drive");
    return;
  }
  fprintf(fp, "%s", data); // write the data to the USB drive
  fclose(fp); // close the file
}



/*
The function periodically checks for the presence of a second USB drive every 5 seconds. If a second USB drive is detected, it checks for any pending files to be written to it. If there are any pending files, it reads the file line by line and writes them to the second USB drive. After writing all pending files, it closes the file and sets the file pointer to NULL. If the second USB drive is not detected, it prints a message and waits for 5 seconds before checking again.
*/


void periodically_check_for_second_usb_drive(FILE *fp1, FILE *fp2, char *buffer) {
  char *usb_path2 = NULL;
  int seconds_to_wait = 5; // specify the interval to check for the second USB drive in seconds
  
  while(1) {
    usb_path2 = identify_usb_drive2(); // identify the second USB drive
    
    if (usb_path2 != NULL) {
      printf("Second USB drive detected at %s\n", usb_path2);
      // check if there are any pending files to be written to the second USB drive
      if (fp2 != NULL) {
        fseek(fp2, 0, SEEK_SET); // move the file pointer to the beginning of the file
        
        // read the file line by line and write to the second USB drive
        while (fgets(buffer, BUFSIZ, fp2)) {
          write_to_usb_drive(buffer, usb_path2);
        }
        
        fclose(fp2); // close the file after writing to the second USB drive
        fp2 = NULL; // set the file pointer to NULL since there are no pending files left
      }
    }
    else {
      printf("Second USB drive not detected. Waiting for %d seconds before checking again.\n", seconds_to_wait);
    }
    
    sleep(seconds_to_wait); // wait for the specified interval before checking for the second USB drive again
  }
}



/*
This function defines a function GetCommandFromRequest that takes a character array request as input and returns an integer corresponding to the command type specified in the request. The function first creates a new character array cmd and copies the input request into it. It then searches for the first space character in the request string and extracts the command type by copying the characters from the start of the request string up to the first space character into the cmd array. The function then uses strcmp to compare the cmd array to a list of predefined command strings and returns the appropriate integer value for the corresponding command. If no valid command is found, the function returns 0.
*/



int GetCommandFromRequest(char* request)
{
	char cmd[CMD_SIZE];
	strcpy(cmd, request);
	int i = 0;
	while(request[i] != ' ' && request[i] != '\0')
		i++;
	if(request[i] == '\0')
		return 6;
	else
	{
		strncpy(cmd, request, i-1);
		cmd[i] = '\0';
	}
		
	if(!strcmp(cmd, "GET"))
		return 1;
	else if(!strcmp(cmd, "PUT"))
		return 2;
	else if(!strcmp(cmd, "MD"))
		return 3;
	else if(!strcmp(cmd, "RM"))
		return 4;
	else if(!strcmp(cmd, "SHOW"))
		return 5;
	else if(!strcmp(cmd, "EXIT"))
		return 6;
	return 0;
}



void performGET(char *file_name, int socket)
{
	char server_response[BUFSIZ];
	printf("Performing GET request of client\n");

	// Check if file present
	if (access(file_name, F_OK) != -1)
	{
		//File is present on server
		//Send "OK" message
		strcpy(server_response, "OK");
		write(socket, server_response, strlen(server_response));
		
		//Send File
		SendFileOverSocket(socket, file_name);
    write_file(socket);
	}
	else
	{

		printf("File not present at server.ABORTING.\n");

		// Requested file does not exist, notify the client
		strcpy(server_response, "NO");
		write(socket, server_response, strlen(server_response)); 
	}
}



/*
The function performs the "PUT" operation for a client-server file transfer. It first checks if the file already exists on the server and notifies the client if it does. It then waits for the client to decide whether to overwrite the existing file or not. If the file does not exist on the server, an "OK" acknowledgement is sent to the client. The code then receives the file size from the client, allocates memory for it, and receives the file data. Finally, it creates a new file, writes the data received from the client to it and closes the file.
*/


void performPUT(char *file_name, int socket)
{
	int c,r;
	printf("Performing PUT request of client\n");

	char server_response[BUFSIZ], client_response[BUFSIZ];
	if(access(file_name, F_OK) != -1)
	{
		// Notifing client that file is present at server
		strcpy(server_response, "FP");
		write(socket, server_response, strlen(server_response));
		
		// Getting the users choice to override or not 
		r = recv(socket, client_response, BUFSIZ, 0);
		client_response[r]='\0';

		if(!strcmp(client_response, "N")){
			printf("User says don't overwrite\n");
			return;
		}
		printf("User says to overwrite the file.\n");

	}
	else
	{
		// Send acknowledgement "OK"
		strcpy(server_response, "OK");
		write(socket, server_response, strlen(server_response));
	}


	// Getting File 
	
	int file_size;
	char *data;
	// Recieving file size and allocating memory
	recv(socket, &file_size, sizeof(int), 0);
	data = malloc(file_size+1);

	// Creating a new file, receiving and storing data in the file.
	FILE *fp = fopen(file_name, "w");
	r = recv(socket, data, file_size, 0);
	data[r] = '\0';
	printf("Size of file recieved is %d\n",r);
	r = fputs(data, fp);
	fclose(fp);
}



/*
The function named performINFO which takes a file path as an argument. The function then uses the stat() function from the sys/stat.h library to retrieve information about the file, including its size, last modification time, owner, group, and permissions. If the stat() function fails to retrieve information about the file, an error message is printed to the console. The function then prints the retrieved information about the file to the console in a human-readable format.

*/


void performINFO(const char* filepath) {
  struct stat filestat;
  if (stat(filepath, &filestat) == -1) {
      perror("Error");
      return;
  }

  printf("File: %s\n", filepath);
  printf("Size: %lld bytes\n", (long long)filestat.st_size);
  printf("Last modified: %s", ctime(&filestat.st_mtime));
  printf("Owner: %d\n", filestat.st_uid);
  printf("Group: %d\n", filestat.st_gid);
  printf("Permissions: %o\n", filestat.st_mode & 0777);
}


/*
The GetArgumentFromRequest function takes a string request that represents a client request, finds the first occurrence of a space character (' ') in the request, and returns a pointer to the character immediately following the space. This effectively extracts the argument portion of the client request.

For example, if the client request is "PUT file.txt", GetArgumentFromRequest would return a pointer to the character 'f', pointing to the start of the file name "file.txt".
*/



char* GetArgumentFromRequest(char* request)
{
	char *arg = strchr(request, ' ');
	return arg + 1;
}

/*
The function called ConnectionHandler which is a thread that handles a new client connection. It receives client requests, extracts the command and arguments from the request, and calls the corresponding function to perform the requested operation.

The code performs the following operations:

Receives the client request
Extracts the command and arguments from the request
Calls the appropriate function based on the command
Sends the result of the operation back to the client
The code supports the following commands:

GET - receives a file from the server
PUT - sends a file to the server
MD - gets the metadata of a file
RM - creates a new folder
SHOW - deletes a file or folder
EXIT - terminates the connection.

*/


// Callback when a new connection is set up
void *ConnectionHandler(void *socket_desc)
{
	int	choice, file_desc, file_size;
	int socket = *(int*)socket_desc;

	char reply[BUFSIZ], file_ext[BUFSIZ],server_response[BUFSIZ], client_request[BUFSIZ], file_name[BUFSIZ];
	char *data;
	while(1)
	{	printf("\nWaiting for command\n");
		int l = recv(socket, client_request, BUFSIZ, 0);
		client_request[l]='\0';
		printf("Command Recieved %s\n",client_request );
		choice = GetCommandFromRequest(client_request);
		switch(choice)
		{
			case 1:
				strcpy(file_name, GetArgumentFromRequest(client_request));
				performGET(file_name, socket);
				break;
			case 2:
				strcpy(file_name, GetArgumentFromRequest(client_request));
				performPUT(file_name, socket);
				break;
			case 3:
				strcpy(file_ext, GetArgumentFromRequest(client_request));
				performINFO(file_name);
				break;
			case 4:
				strcpy(file_name, GetArgumentFromRequest(client_request));
        int status = mkdir(file_name, 0700); // create folder
        if (status != 0) {
          strcpy(reply, "Failed to create folder.\n");
        } else {
          strcpy(reply, "Folder created successfully.\n");
        }
        send(socket, reply, strlen(reply), 0);
				break;
			case 5:
				// Delete file/folder
        strcpy(file_name, GetArgumentFromRequest(client_request));
        int status = remove(file_name);
        if (status != 0) {
          strcpy(reply, "Failed to delete file/folder.\n");
        } else {
          strcpy(reply, "File/folder deleted successfully.\n");
        }
        send(socket, reply, strlen(reply), 0);
				break;
			case 6:
				free(socket_desc);   
				return 0;
		}
	}
	free(socket_desc);   
	return 0;
}


/*
This code creates a socket and listens for incoming connections on a specified IP address and port number. It creates a socket, binds it to the IP address and port number, and listens for incoming connections. When a connection is accepted, it creates a new thread or process to handle the connection. If a thread is created, it calls the ConnectionHandler function, and if a process is created, it writes data to file1.txt and periodically checks for a second USB drive, calls the performINFO function and then calls the ConnectionHandler function.The code takes two command-line arguments: the port number and the IP address to bind to. If the command-line arguments are not provided, the program exits. Finally, it closes the socket and returns 0.
*/



int main(int argc , char *argv[])
{
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];
  FILE *fp1 = fopen("file1.txt", "r");
  FILE *fp2 = fopen("file2.txt", "r");
  char buffer[BUFSIZ];


  if(argc!=3){
		printf("Invalid arguments\n");
		return 0;
	}

  //to pass port and address number through command line please use port number 9765 and use "127.0.0.1"
  int port = atoi(argv[1]);
  int address_number = atoi(argv[2]);
  
  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));
  
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_desc < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(address_number);
  
  // Bind to the set port and IP:
  if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(socket_desc, 1) < 0){
    printf("Error while listening\n");
    return -1;
  }
  printf("\nListening for incoming connections.....\n");

  while(1){
    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

    if (client_sock < 0){
      printf("Can't accept\n");
      return -1;
    }
    
    printf("Client connected at IP: %s and port: %i\n", 
           inet_ntoa(client_addr.sin_addr), 
           ntohs(client_addr.sin_port));


    
    // Create a new thread or process to handle the connection
    #ifdef THREAD
    pthread_t thread_id;
    struct thread_args *args = malloc(sizeof(struct thread_args));
    args->client_sock = client_sock;
    args->fp1 = fp1;
    args->fp2 = fp2;
    if (pthread_create(&thread_id, NULL, ConnectionHandler, (void*)&client_sock) < 0) {
      printf("Error creating thread\n");
      return -1;
    }
    #else
    pid_t pid = fork();
    if (pid == -1) {
      printf("Error forking process\n");
      return -1;
    } else if (pid == 0) {
      // Child process: handle the connection
      write_file(client_sock);
      periodically_check_for_second_usb_drive(fp1, fp2, buffer);
      printf("Data written in the file successfully.\n");
      performINFO("folder/path/file.txt");
      ConnectionHandler((void*)&client_sock);
      close(client_sock);
      return 0;
    }
    // Parent process: continue listening for new connections
    #endif

  }
  
  
  // Closing the socket:
  close(socket_desc);
  
  return 0;
}
