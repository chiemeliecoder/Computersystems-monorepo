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

void write_file(int socket_desc){
  int n;
  FILE *fp;
  char *filename = "file2.txt";
  char buffer[BUFFSIZE];

  fp = fopen(filename, "w");
  while (1) {
    // Receive client's message:
    n = recv(socket_desc, buffer, BUFFSIZE, 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, BUFFSIZE);
  }
  return;
}




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




char* GetArgumentFromRequest(char* request)
{
	char *arg = strchr(request, ' ');
	return arg + 1;
}




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
				//	performMPUT(socket);
				break;
			case 5:
				// showFile(socket);
				break;
			case 6:
				free(socket_desc);   
				return 0;
		}
	}
	free(socket_desc);   
	return 0;
}






int main(int argc , char *argv[])
{
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];
  char buffer[BUFFSIZE];


  if(argc!=2){
		printf("Invalid arguments\n");
		return 0;
	}
  
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
  server_addr.sin_port = htons(9765);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
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
  
  // Accept an incoming connection:
  client_size = sizeof(client_addr);
  client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

  write_file(client_sock);
  printf("Data written in the file successfully.\n");
  performINFO("folder/path/file.txt");

  
  if (client_sock < 0){
    printf("Can't accept\n");
    return -1;
  }
  printf("Client connected at IP: %s and port: %i\n", 
         inet_ntoa(client_addr.sin_addr), 
         ntohs(client_addr.sin_port));
  
  
  
  // Closing the socket:
  close(client_sock);
  close(socket_desc);
  
  return 0;
}
