#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/wait.h> // for wait()
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



// Instantiate a mutex to synchronize access to the USB devices
pthread_mutex_t usb_mutex = PTHREAD_MUTEX_INITIALIZER;
// Instantiate a mutex for open file pointers for both USB devices
pthread_mutex_t fd_mutex = PTHREAD_MUTEX_INITIALIZER;
// Instantiate a mutex for creating new thread for each client
pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
The function is used to get information about a remote file and send it back to a client over a network connection.The function takes two arguments, the first one is an integer client_sock which is the socket descriptor of the connected client and the second one is a pointer to a constant character remote_file_path which represents the path of the remote file that we want to get information about.
*/
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




// function to handle the PUT command which performs a file transfer from the local machine to two remote machines over a network. The function takes three arguments: an integer representing the client socket, a string representing the path of the remote file, and a string representing the path of the local file.opens the local file for reading and creates two remote files for writing. If the remote file path is not provided, it uses the local file name as the remote file name. It then receives the expected file size from the client over the network and proceeds to copy the content of the local file to the two remote files. 

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




// Define the client_handler function  is designed to handle a single client connection in a server program and receives a client socket file descriptor as an argument, reads data from the client, parses the client message to extract the command and file paths, and performs the appropriate action based on the command.

void *client_handler(void *arg) {
  int client_sock = *(int*)arg;
  char client_message[BUFFER_SIZE];
  ssize_t read_size;

  // Receive client message
  ssize_t total_bytes_read = 0;
  while (total_bytes_read < BUFFER_SIZE) {
    ssize_t bytes_read = recv(client_sock, client_message + total_bytes_read, BUFFER_SIZE - total_bytes_read, 0);
    if (bytes_read <= 0) {
      printf("Error while receiving client message\n");
      close(client_sock);
      return NULL;
    }
    total_bytes_read += bytes_read;
  }

  // Check the client message and perform the appropriate command
  char command[BUFFER_SIZE], remote_file_path[BUFFER_SIZE], local_file_path[BUFFER_SIZE];
  sscanf(client_message, "%s %s %s", command, remote_file_path, local_file_path);

   // Read data from client
  read_size = read(client_sock, client_message, BUFFER_SIZE);
  if (read_size < 0) {
    printf("Error while reading from client\n");
    close(client_sock);
    return NULL;
  }

   // Make a copy of the message before calling strtok()
  char message_copy[BUFFER_SIZE];
  strcpy(message_copy, client_message);
  
  // Extract command and remote file path from client message:
  char* token = strtok(message_copy, " ");
  if (token == NULL) {
    printf("Invalid client message\n");
    close(client_sock);
    return NULL;
  }
  strcpy(command, token);
  
  token = strtok(NULL, " ");
  if (token == NULL) {
    printf("Invalid client message\n");
    close(client_sock);
    return NULL;
  }
  strcpy(remote_file_path, token);
  
  token = strtok(NULL, " ");
  if (token == NULL) {
    printf("Invalid client message\n");
    close(client_sock);
    return NULL;
  }
  strcpy(local_file_path, token);

  

  if (strcmp(command, "INFO") == 0) {
    perform_info_command(client_sock, remote_file_path);
  } else if (strcmp(command, "PUT") == 0) {
    perform_put_command(client_sock, remote_file_path, local_file_path);
  } else if (strcmp(command, "GET") == 0) {
    // Open file pointers for both USB devices
    int fd1 = open("/dev/sdb1", O_RDONLY);
    int fd2 = open("/dev/sdc1", O_RDONLY);

    if (fd1 < 0 || fd2 < 0) {
      // Error if either device cannot be opened
      printf("Error opening USB devices\n");
      //close(client_sock);
      //return NULL;
    }
    // Open file and read its contents into a buffer:
    FILE* file = fopen(remote_file_path, "rb");
    if (file == NULL) {
      printf("Error while opening remote file\n");
      close(client_sock);
      return NULL;
    }
    // Read data from both devices simultaneously
    ssize_t bytes_read1 = read(fd1, client_message, BUFFER_SIZE/2);
    ssize_t bytes_read2 = read(fd2, client_message + BUFFER_SIZE/2, BUFFER_SIZE/2);
    if (bytes_read1 < 0 || bytes_read2 < 0) {
      // Error if either device cannot be read
      printf("Error reading from USB devices\n");
      close(fd1);
      close(fd2);
      close(client_sock);
      return NULL;
    }
    
    // Send data to client
    ssize_t total_bytes_sent = 0;
    while (total_bytes_sent < bytes_read1 + bytes_read2) {
      ssize_t bytes_sent = send(client_sock, client_message + total_bytes_sent, bytes_read1 + bytes_read2 - total_bytes_sent, 0);
      if (bytes_sent < 0) {
        printf("Error while sending data to client\n");
        // Close file pointers
        close(fd1);
        close(fd2);
        close(client_sock);
        return NULL;
      }
      total_bytes_sent += bytes_sent;
    }

    if (total_bytes_sent < bytes_read1 + bytes_read2) {
      printf("Not all bytes were sent to client\n");
      close(fd1);
      close(fd2);
      close(client_sock);
      return NULL;
    }
    
  }else {
    char response[BUFFER_SIZE];
    snprintf(response, BUFFER_SIZE, "Unknown command: %s", command);
    send(client_sock, response, strlen(response), 0);
  }

  // Close the client socket and return
  close(client_sock);
  return NULL;
}



/**
The implementation below is of a remote file management system. It listens for incoming connections and accepts file management commands from clients over a socket connection. Upon connection, it checks if two USB devices (/dev/sdb1 and /dev/sdc1) are available and can be written to. If both devices are available, it writes data from a buffer to both devices. It then creates a new thread to handle the client, detaches the thread and receives the client message which contains a command, a remote file path, and a local file path. It then extracts the command and file paths, and performs the appropriate action depending on the command
*/


int main(int argc , char *argv[])
{
  int socket_desc, client_sock, read_size;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];
  char response[BUFFER_SIZE];
  char buffer[BUFFER_SIZE];
  int nbytes = sizeof(buffer);

  if(argc!=2){
		printf("Usage: %s <port>\n",argv[0]);
		return -1;
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


     // Check if USB devices are still available
    int lock_result = pthread_mutex_trylock(&usb_mutex);
    if (lock_result != 0) {
      printf("Error: Unable to lock USB mutex\n");
      close(client_sock);
      continue;
    }
    
    if (access("/dev/sdb1", F_OK) == -1 || access("/dev/sdc1", F_OK) == -1) {
      printf("Error: USB devices not available\n");
      pthread_mutex_unlock(&usb_mutex);
      close(client_sock);
      continue;
    }

    pthread_mutex_unlock(&usb_mutex);

    
    // Open file pointers for both USB devices
    lock_result = pthread_mutex_trylock(&fd_mutex);
    if (lock_result != 0) {
      printf("Error: Unable to lock file descriptor mutex\n");
      close(client_sock);
      continue;
    }
    int fd1 = open("/dev/sdb1", O_WRONLY);
    int fd2 = open("/dev/sdc1", O_WRONLY);
    if (fd1 < 0 || fd2 < 0) {
      // Error if either device cannot be opened
      printf("Error opening USB devices\n");
      pthread_mutex_unlock(&fd_mutex);
      return -1;
    }
    pthread_mutex_unlock(&fd_mutex);


    // Write data to both devices simultaneously
    ssize_t bytes_written1 = write(fd1, buffer, nbytes);
    ssize_t bytes_written2 = write(fd2, buffer, nbytes);
    if (bytes_written1 != nbytes || bytes_written2 != nbytes) {
      // Error if the number of bytes written to either device is not equal to nbytes
      printf("Error writing to USB devices\n");
      return -1;
    }
    
    // Close file pointers
    close(fd1);
    close(fd2);

    // Create a new thread for each client
    lock_result = pthread_mutex_trylock(&socket_mutex);
    if (lock_result != 0) {
      printf("Error: Unable to lock socket mutex\n");
      close(client_sock);
      continue;
    }
    pthread_t client_thread;
    if (pthread_create(&client_thread, NULL, client_handler, &client_sock) < 0) {
      printf("Error while creating thread\n");
      pthread_mutex_unlock(&socket_mutex);
      return -1;
    }

    printf("New thread created to handle the client\n");

    // Detach the thread so it can run independently:
    if (pthread_detach(client_thread) < 0) {
      printf("Error while detaching thread\n");
      pthread_mutex_unlock(&socket_mutex);
      return -1;
    }
    pthread_mutex_unlock(&socket_mutex);

    
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
