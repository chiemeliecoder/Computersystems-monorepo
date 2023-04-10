/*
 * client.c -- TCP Socket Client
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <linux/usbdevice_fs.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#define BUFFSIZE 1024
#define MAXFILE 100
#define FILENAME 100

char *usb_path; // global variable to store USB drive path


// I used BUFSIZ instead of the BUFFSIZE I defined because It is generally better to use the predefined constant BUFSIZ instead of defining your own buffer size constant BUFFSIZE. This ensures that the buffer size is set to a reasonable value that is suitable for the system on which the program is running. 


void send_file(const char *file_path, int socket_desc);
void performGET(char *file_name,int socket_desc);
void performPUT(char *file_name,int socket_desc);
char *identify_usb_drive();




/*
The function is sending a file over a socket connection and writing it to a destination file in a USB drive. It reads the file in chunks, sends each chunk over the socket, and writes it to the destination file. The code also checks for errors during the process and exits the program if any errors occur.
*/




void send_file(const char *file_path, int socket_desc){
  int n;
  char data[BUFFSIZE] = {0};
  FILE *fp;
  

  fp = fopen(file_path, "r");
  if (fp == NULL) {
    perror("Error in reading file.");
    exit(1);
  }


    // Get the USB drive path
  char *usb_path = identify_usb_drive();
  if (usb_path == NULL) {
    printf("No USB drive found.\n");
    exit(1);
  }

  // Create the destination file in the USB drive for writing
  char destination_path[256];
  sprintf(destination_path, "%s/%s", usb_path, file_path);
  FILE *destination_file = fopen(destination_path, "wb");
  if (destination_file == NULL) {
    printf("Error: Unable to create destination file in USB drive\n");
    fclose(fp);
    free(usb_path);
    exit(1);
  }

  // Copy data from the source file to the destination file
  while(fgets(data, BUFSIZ, fp) != NULL) {
    // Send the message to server:
    if (send(socket_desc, data, sizeof(data), 0) == -1) {
      perror("Error in sending file.");
      fclose(fp);
      fclose(destination_file);
      free(usb_path);
      exit(1);
    }
    // Write the data to the destination file in the USB drive
    fwrite(data, sizeof(char), strlen(data), destination_file);
    bzero(data, BUFSIZ);
  }

  // Close both files
  fclose(fp);
  fclose(destination_file);
  free(usb_path);
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
The identify_usb_drive() function identifies the path to a USB drive by scanning the "/media" directory and checking if each directory in it corresponds to a USB drive using the is_usb_drive() function. If a USB drive is found, it returns the path to the drive, otherwise, it returns NULL.
*/



char* identify_usb_drive() {
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
The function below defines a function performGET that performs a GET request to a server to retrieve a file with a given file name. It first checks if the file already exists locally and prompts the user to overwrite it or abort the operation. If the file is not present locally or the user decides to overwrite it, the function sends a GET request to the server with the given file name. If the file exists at the server, the function receives the file size and data and writes the data to a local file with the same name as the requested file. If the file does not exist at the server, the function prints an error message.
*/




void performGET(char *file_name,int socket_desc){
	char request_msg[BUFSIZ], reply_msg[BUFSIZ];
	int file_size;
	char *data;
	int t;
	if( access( file_name, F_OK ) != -1 )
	{
		int abortflag = 0;
		printf("File already exists locally. Press 1 to overwrite. Press any other key to abort.\n");
		scanf("%d", &abortflag);
		if(abortflag!=1)
			return;
	}
	// Get a file from server
	strcpy(request_msg, "GET ");
	strcat(request_msg, file_name);
	write(socket_desc, request_msg, strlen(request_msg));
	recv(socket_desc, reply_msg, 2, 0);
	reply_msg[2] = '\0';
	printf("%s\n", reply_msg);
	if (strcmp(reply_msg, "OK") == 0)
	{
	
		// File present at server.Start receiving the file and storing locally.
		printf("Recieving data\n");
	
		recv(socket_desc, &file_size, sizeof(int), 0);
		data = malloc(file_size+1);
		FILE *fp = fopen(file_name, "w"); 
		t = recv(socket_desc, data, file_size, 0);
		data[t] = '\0';
		fputs(data, fp);
		fclose(fp);
		printf("File %s recieved with size %d \n", file_name,t);
	}
	else
	{
		printf("File doesn't exist at server.ABORTING.\n");
	}
}



/*
The function is implementing a "PUT" operation in a client-server file transfer protocol. The client sends a PUT request to the server with the file name, and if the server does not have the file, the client sends the file to the server. If the file already exists on the server, the client prompts the user whether to overwrite or not. If the user chooses to overwrite, the client sends the file to the server. If the user chooses not to overwrite or if the server cannot create the file, the transfer is aborted.
*/



void performPUT(char *file_name,int socket_desc)
{
	int	file_size, file_desc,c,t;
	char *data;
	char request_msg[BUFSIZ], reply_msg[BUFSIZ],client_response[2];
	// Get a file from server
	strcpy(request_msg, "PUT ");
	strcat(request_msg, file_name);
	printf("Trying to PUT %s to server. \n",file_name );
	if (access(file_name, F_OK) != -1)
	{
		// Sending PUT request to server.
		write(socket_desc, request_msg, strlen(request_msg));
		t = recv(socket_desc, reply_msg, BUFSIZ, 0);
		reply_msg[t]='\0';
		if (strcmp(reply_msg, "OK") == 0)
		{
			// Everything if fine and send file
			send_file(file_name, socket_desc);
		}
		else if(strcmp(reply_msg, "FP") == 0)
		{
			// File present at server.
			printf("File exists in server. Do you wan't to overwrite? 1/0\n");
			scanf("%d", &c);
			if(c)
			{
				// User says yes to overwrite. Send Y and then data
				printf("Overwriting %s\n",file_name );
				strcpy(client_response, "Y");
				write(socket_desc, client_response, strlen(client_response));
				send_file(file_name, socket_desc);
			}
			else
			{
				printf("Not sending %s file to server\n",file_name);
				// User says No to overwrite. Send N and exit
				strcpy(client_response, "N");
				write(socket_desc, client_response, strlen(client_response));
				return;
			}
		}
		else{
			// Server can't create file.
			printf("Server can't create file...\n");
		}
	}
	else
	{
		// File not found locally hence abort.
		printf("File not found locally...\n");
		return;
	}
}



/*
The function is implementing a client-side application that can communicate with a server. It establishes a connection with the server, sends a file to the server using the send_file() function, and then waits for user input to perform various actions on the server-side files such as GET, PUT, MD, RM, and EXIT. For example, when the user chooses to create a directory, the mkdir() function is used to create the directory on the server-side, and when the user chooses to delete a file or a directory, the remove() function is used to remove it from the server-side. The connection is closed at the end of the program.
*/




int main(int argc , char *argv[])
{

  
  int socket_desc;
  struct sockaddr_in server_addr;
  char server_message[2000], client_message[2000];
  char *filename = "file1.txt";


  if(argc!=3){
		printf("Invalid arguments\n");
		return 0;
	}

  
  
  // Clean buffers:
  memset(server_message,'\0',sizeof(server_message));
  memset(client_message,'\0',sizeof(client_message));
  
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_desc < 0){
    printf("Unable to create socket\n");
    return -1;
  }
  
  printf("Socket created successfully\n");
  
  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9765);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  // Send connection request to server:
  if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    printf("Unable to connect\n");
    return -1;
  }
  printf("Connected with server successfully\n");
  
  
  

  send_file(filename, socket_desc);
  printf("File data sent successfully.\n");


  int choice = 0;
  char folder_name[50]; // define folder_name variable
	while(1)
	{
		printf("Enter a choice:\n1- GET\n2- PUT\n3- MD\n4- RM\n5- EXIT\n");
		scanf("%d", &choice);
		switch(choice)
		{
			case 1:
				printf("Enter file_name to get: ");
				scanf("%s", filename);
				performGET(filename,socket_desc);
				break;
			case 2:
				printf("Enter file_name to put: ");
				scanf("%s", filename);
				performPUT(filename,socket_desc);
				break;
      case 3:
        printf("Enter folder_name to create: ");
        scanf("%s", folder_name);
        int status = mkdir(folder_name, 0700); // create folder
        if (status != 0) {
          printf("Error creating folder\n");
        } else {
          printf("Folder created successfully\n");
        }
        break;
      case 4:
        printf("Enter file/folder name to delete: ");
        scanf("%s", filename);
        int status = remove(filename);
        if (status == 0) {
          printf("File/folder deleted successfully.\n");
        } else {
          printf("Failed to delete file/folder.\n");
        }
        break;
			case 5:
				strcpy(request_msg,"EXIT");
				write(socket_desc, request_msg, strlen(request_msg));	
				return 0;
			default: 
				printf("Incorrect command\n");
		}
	}
 

  
  
  // Close the socket:
  printf("Closing the connection.\n");
  close(socket_desc);
  
  return 0;
}
