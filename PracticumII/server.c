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




// function to handle the PUT command
void perform_put_command(int client_sock, char* remote_file_path, char* local_file_path) {
  FILE* local_file;
  FILE* remote_file;
  FILE* remote_file2;
  char buffer[BUFFER_SIZE];
  size_t bytes_read;
  int total_bytes_received = 0;
  int expected_file_size;

  // Open the local file
  if (local_file_path == NULL) {
    printf("Local file path not provided, using remote file path as local file path\n");
    local_file_path = remote_file_path;
  }
  local_file = fopen(local_file_path, "rb");
  if (local_file == NULL) {
    printf("Error while opening local file\n");
    return;
  }

  // Create the remote file
  if (remote_file_path == NULL) {
    printf("Remote file path not provided, using local file name as remote file name\n");
    char* file_name = strrchr(local_file_path, '/');
    if (file_name == NULL) {
      printf("Error: Local file name not found\n");
      fclose(local_file);
      return;
    }
    remote_file_path = file_name + 1;
  }
  remote_file = fopen(strcat(remote_file_path, "1"), "wb");
  remote_file2 = fopen(strcat(remote_file_path, "2"), "wb");
  if (remote_file == NULL || remote_file2 == NULL) {
    printf("Error while creating remote file\n");
    fclose(local_file);
    if (remote_file != NULL) fclose(remote_file);
    if (remote_file2 != NULL) fclose(remote_file2);
    return;
  }

  // Get the expected file size from the client
  if (recv(client_sock, &expected_file_size, sizeof(expected_file_size), 0) < 0) {
    printf("Error while receiving file size from client\n");
    fclose(local_file);
    fclose(remote_file);
    fclose(remote_file2);
    return;
  }

  // Copy the content of the local file to the remote file
  while (total_bytes_received < expected_file_size && (bytes_read = fread(buffer, 1, sizeof(buffer), local_file)) > 0) {
    if (fwrite(buffer, 1, bytes_read, remote_file) != bytes_read || fwrite(buffer, 1, bytes_read, remote_file2) != bytes_read) {
      printf("Error while writing to remote file\n");
      break;
    }
  }

  // Close the files
  fclose(local_file);
  fclose(remote_file);
  fclose(remote_file2);

  // Send the response to the client
  char response[BUFFER_SIZE];
  sprintf(response, "File %s created successfully on both USB devices\n", remote_file_path);
  if (send(client_sock, response, strlen(response), 0) < 0) {
    printf("Can't send response to client\n");
    return;
  }
}





int main(int argc , char *argv[])
{
  int socket_desc, client_sock, read_size;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];
  char response[BUFFER_SIZE];

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
    }else if (strcmp(command, "MD") == 0){
      // Create the folder using mkdir():
      if (mkdir(remote_file_path, 0777) == 0) {
        sprintf(response + strlen(response), "Directory created successfully\n");
      } else {
          sprintf(response + strlen(response), "Error creating directory\n");
      }
  
      // Send the response to the client
      if (send(client_sock, response, strlen(response), 0) < 0){
          printf("Can't send\n");
          return -1;
      }
  
      printf("Response sent: %s\n", response);
      close(client_sock);
      return 0;
      
    }else if(strcmp(command, "RM") == 0){
       // Check if the remote file path is a file or directory:
      struct stat s;
      if (stat(remote_file_path, &s) < 0) {
        sprintf(response + strlen(response), "Error getting file or directory information\n");
      } else {
        if (S_ISREG(s.st_mode)) {
          // Remove the file using unlink():
          if (unlink(remote_file_path) == 0) {
            sprintf(response + strlen(response), "File deleted successfully\n");
          } else {
            sprintf(response + strlen(response), "Error deleting file\n");
          }
        } else if (S_ISDIR(s.st_mode)) {
          // Remove the directory using rmdir():
          if (rmdir(remote_file_path) == 0) {
            sprintf(response + strlen(response), "Directory deleted successfully\n");
          } else {
            sprintf(response + strlen(response), "Error deleting directory\n");
          }
        } else {
          sprintf(response + strlen(response), "Unknown file type\n");
        }
      }
      
      // Send the response to the client
      if (send(client_sock, response, strlen(response), 0) < 0){
        printf("Can't send\n");
        return -1;
      }
    
      printf("Response sent: %s\n", response);
      close(client_sock);
      return 0;
    }else if(strcmp(command, "PUT") == 0){
      // Call perform_put_command() to create the file and store the data:
      perform_put_command(client_sock, remote_file_path, local_file_path);
    }else{
      printf("Unknown command\n");
      return -1;
    }
  }
  
  close(client_sock);
  
  return 0;
}
