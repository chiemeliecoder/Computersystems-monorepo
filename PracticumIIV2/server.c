#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "server.h"


#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


void perform_info_command(int client_sock, const char* remote_file_path) {
  char response[BUFFER_SIZE];
  struct stat file_info;
  
  // Get file information using stat():
  if (stat(remote_file_path, &file_info) == -1) {
    snprintf(response, BUFFER_SIZE, "Error getting file information: %s", strerror(errno));
    send(client_sock, response, strlen(response), 0);
    return;
  }
  
  // Format the response with the file information:
  snprintf(response, BUFFER_SIZE,
           "File name: %s\n"
           "Size: %ld bytes\n"
           "Owner: %d\n"
           "Group: %d\n"
           "Permissions: %o\n"
           "Last modified: %s",
           remote_file_path,
           file_info.st_size,
           file_info.st_uid,
           file_info.st_gid,
           file_info.st_mode & 0777,
           ctime(&file_info.st_mtime));
  
  // Send the response back to the client:
  send(client_sock, response, strlen(response), 0);
}



int main(int argc , char *argv[])
{
  int socket_desc, client_sock, read_size;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];

  if(argc!=2){
		printf("Invalid arguments\n");
		return 0;
	}
   //to pass port number through command line please use port number
  int PORTS = atoi(argv[1]);
  
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
  server_addr.sin_port = htons(PORTS);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  // Bind to the set port and IP:
  if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    printf("Error while binding\n");
    return -1;
  }
  printf("Binding done successfully\n");
  
  // Listen for incoming connections:
  if(listen(socket_desc, 1) < 0){
    printf("Error while listening\n");
    return -1;
  }

  while(1){
    // Accept incoming connection:
    printf("Waiting for incoming connections...\n");
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
      printf("Error while accepting connection\n");
      return -1;
    }
    printf("Connection accepted successfully\n");
  
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr),   ntohs(client_addr.sin_port));
    
    // Receive command and remote file path:
    read_size = recv(client_sock, client_message, sizeof(client_message), 0);
    
    if (read_size < 0) {
      printf("Error while receiving command and remote file path\n");
      return -1;
    }
    
    // Extract command and remote file path from client message:
    char* command = strtok(client_message, " ");
    char* remote_file_path = strtok(NULL, " ");
    char* local_file_path = strtok(NULL, " ");
  
  
    // Check if the command is GET:
    if (strcmp(command, "GET") == 0) {
      // Open file and send data:
      FILE* file = fopen(remote_file_path, "rb");
      if (file == NULL) {
        printf("Error while opening remote file\n");
        return -1;
      }
  
  
      // Read file data and send to client:
      int bytes_read;
      while ((bytes_read = fread(server_message, sizeof(char), sizeof(server_message), file)) > 0) {
        if (send(client_sock, server_message, bytes_read, 0) < 0){
          printf("Can't send file data\n");
          return -1;
        }
        memset(server_message, '\0', sizeof(server_message));
      }
      // Close file and client socket:
      fclose(file);
    }else if (strcmp(command, "INFO") == 0){
      // Call perform_info_command() to get file information and send it to the client:
      perform_info_command(client_sock, remote_file_path);
    }else{
      printf("PUT command\n");
      return -1;
    }
  }
  
  close(client_sock);
  
  return 0;
}
