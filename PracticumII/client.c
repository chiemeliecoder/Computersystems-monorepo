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


void send_file(const char *file_path, int socket_desc);
void performGET(char *file_name,int socket_desc);
void performPUT(char *file_name,int socket_desc);
char *identify_usb_drive();





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
  while(fgets(data, BUFFSIZE, fp) != NULL) {
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
    bzero(data, BUFFSIZE);
  }

  // Close both files
  fclose(fp);
  fclose(destination_file);
  free(usb_path);
}
  




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
			SendFileOverSocket(socket_desc, file_name);
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
				SendFileOverSocket(socket_desc, file_name);
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
				scanf("%s", file_name);
				performGET(file_name,socket_desc);
				break;
			case 2:
				printf("Enter file_name to put: ");
				scanf("%s", file_name);
				performPUT(file_name,socket_desc);
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
        scanf("%s", file_name);
        int status = remove(file_name);
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
