#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include "fgets.h"




int main(int argc, char* argv[])
{
  if (argc < 3) {
    printf("Usage: %s <command> <remote_file_path> <local_file_path>\n", argv[0]);
    return -1;
  }


  char* command = argv[1];
  char* remote_file_path = argv[2];
  char* local_file_path = argv[3];


  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char client_message[8196];

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");

  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8958);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect to server:
  if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    printf("Error while connecting to server\n");
    return -1;
  }
  printf("Connected to server successfully\n");
  
  // Send command and remote file path:
  char request[8196];
  if (strcmp(command, "GET") == 0) {
    sprintf(request, "%s %s", command, remote_file_path);  
  }
  else if (strcmp(command, "INFO") == 0) {
    sprintf(request, "%s %s", command, remote_file_path);
    // Get file information using stat():
    struct stat file_info;
    if (stat(remote_file_path, &file_info) < 0) {
      printf("Error while getting file information\n");
      return -1;
    }

    // Build response string with file information:
    char response[8196];
    sprintf(response, "Filename: %s\n", remote_file_path);
    sprintf(response + strlen(response), "Size: %ld bytes\n", file_info.st_size);
    sprintf(response + strlen(response), "Owner: %d\n", file_info.st_uid);
    sprintf(response + strlen(response), "Group: %d\n", file_info.st_gid);
    sprintf(response + strlen(response), "Permissions: %o\n", file_info.st_mode & 0777);
    sprintf(response + strlen(response), "Last modified: %s", ctime(&file_info.st_mtime));
    // Add any additional information you want to include here

    // Send response string to client:
    if (send(socket_desc, response, strlen(response), 0) < 0){
      printf("Can't send\n");
      return -1;
    }

    printf("File information sent: %s\n", response);
    close(socket_desc);
    return 0;
  }
  else if (strcmp(command, "MD") == 0) {
    sprintf(request, "%s %s", command, remote_file_path);
     // Create the directory using mkdir()
    if (mkdir(remote_file_path, 0777) == 0) {
        sprintf(request + strlen(request), "Directory created successfully\n");
    } else {
        sprintf(request + strlen(request), "Error creating directory\n");
    }

    // Send the response to the client
    if (send(client_sock, request, strlen(request), 0) < 0){
        printf("Can't send\n");
        return -1;
    }

    printf("Response sent: %s\n", request);
    close(client_sock);
    return 0;
  }else if(strcmp(command, "RM") == 0){
    sprintf(request, "%s %s", command, remote_file_path);
    // Remove the file or directory using remove()
    if (remove(remote_file_path) == 0) {
      sprintf(request + strlen(request), "File or directory deleted successfully\n");
    } else {
      sprintf(request + strlen(request), "Error deleting file or directory\n");
    }

    // Send the response to the client
    if (send(client_sock, request, strlen(request), 0) < 0){
      printf("Can't send\n");
      return -1;
    }

    printf("Response sent: %s\n", request);
    close(client_sock);
    return 0;
    
  }else if(strcmp(command, "PUT") == 0){
    if (argc == 3) {
        local_file_path = argv[2];
        remote_file_path = argv[1];
    } else {
        local_file_path = remote_file_path;
        remote_file_path = argv[1];
    }

    FILE* fp = fopen(local_file_path, "rb");
    if (!fp) {
        printf("Can't open file\n");
        return -1;
    }

    // Get file size:
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Build request string with command, remote file path, and file size:
    char request[8196];
    sprintf(request, "%s %s %d", command, remote_file_path, file_size);

    // Send request to server:
    if (send(socket_desc, request, strlen(request), 0) < 0){
        printf("Can't send\n");
        return -1;
    }

    // Receive response from server:
    char response[8196];
    if (recv(socket_desc, response, sizeof(response), 0) < 0) {
      printf("Can't receive\n");
      return -1;
    }

    // Check if server is ready to receive file data:
    if (strcmp(response, "READY") != 0) {
      printf("Server not ready: %s\n", response);
      return -1;
    }

    // Send file data to server:
    char file_data[8196];
    int total_bytes = 0;
    int bytes_read;
    while ((bytes_read = fread(file_data, 1, sizeof(file_data), fp)) > 0) {
      int bytes_sent = send(socket_desc, file_data, bytes_read, 0);
      if (bytes_sent < 0) {
        printf("Can't send file data\n");
        return -1;
      }
      total_bytes += bytes_sent;
    }

    // Close file and socket:
    fclose(fp);
    close(socket_desc);

    printf("File sent successfully\n");
    return 0;
    
  }
  else {
    sprintf(request, "%s %s %s", command, remote_file_path, local_file_path);
  }

  if (send(socket_desc, request, strlen(request), 0) < 0){
    printf("Can't send\n");
    return -1;
  }
  
  printf("Request sent: %s\n", request);

  // Receive file data:
  char file_data[8196];
  int total_bytes = 0;
  int bytes_received;
  while ((bytes_received = recv(socket_desc, file_data + total_bytes, sizeof(file_data) - total_bytes, 0)) > 0) {
    total_bytes += bytes_received;
  }
  if (bytes_received < 0) {
    printf("Error while receiving file data\n");
    return -1;
  }
  file_data[bytes_received] = '\0';

  // Print response string to console:
  printf("%s\n", file_data);
  
  // Save file data to local file:
  FILE* file = fopen(local_file_path, "wb");
  if (file == NULL) {
    printf("Error while opening local file\n");
    return -1;
  }
  if (fwrite(file_data, sizeof(char), total_bytes, file) < total_bytes) {
    printf("Error while writing to local file\n");
    return -1;
  }
  fclose(file);
  printf("File saved to: %s\n", local_file_path);
  // Closing the socket:
  close(socket_desc);
}
