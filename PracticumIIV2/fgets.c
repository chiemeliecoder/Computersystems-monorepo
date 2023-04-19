#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include "fgets.h"




//the function read_from_usb that reads data from a USB device and writes it to a file.
void* read_from_usb(void* arg) {
  struct thread_data* data = (struct thread_data*) arg;

  FILE* fp = fopen(data->usb_device, "rb");
  if (!fp) {
    printf("Error opening USB device %s\n", data->usb_device);
    pthread_exit(NULL);
  }

  FILE* f = fopen(data->file_path, "wb");
  if (!f) {
    printf("Can't open file %s\n", data->file_path);
    fclose(fp);
    pthread_exit(NULL);
  }

  int read_bytes;
  char buffer[4096];
  while ((read_bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
    fwrite(buffer, 1, read_bytes, f);
  }

  fclose(fp);
  fclose(f);

  printf("Finished reading from USB device %s and writing to file %s\n", data->usb_device, data->file_path);

  pthread_exit(NULL);
}







/**
 The command-line arguments and performs various operations based on the command specified. The program takes at least 3 arguments: the command to execute (e.g., GET, INFO, MD, RM, PUT), the remote file path, and the local file path (for GET and PUT).The program then checks if two USB devices are available by calling the stat function on /dev/sdb1 and /dev/sdc1.If the USB devices are available, the program will create a TCP socket and connect to a server running on port 8958.
*/


int main(int argc, char* argv[])
{
  
  if (argc < 3) {
    printf("Usage: %s <command> <remote_file_path> <local_file_path>\n", argv[0]);
    return -1;
  }

  struct stat sdb1, sdc1;
  int sdb1_stat, sdc1_stat;
  
  sdb1_stat = stat("/dev/sdb1", &sdb1);
  sdc1_stat = stat("/dev/sdc1", &sdc1);
  
  if (sdb1_stat != 0 || sdc1_stat != 0) {
    printf("One or both of the USB devices are not available.\n");
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
    struct thread_data data1 = {"/dev/sdb1", remote_file_path};
    struct thread_data data2 = {"/dev/sdc1", remote_file_path};

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, read_from_usb, (void*) &data1);
    pthread_create(&thread2, NULL, read_from_usb, (void*) &data2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Files read from USB devices and saved to local files\n");
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

    bool fp1_available = true;  // flag to keep track of the availability of the first USB device
    bool fp2_available = true;  // flag to keep track of the availability of the second USB device
     // Open file pointers for both USB devices
    FILE* fp1 = fopen("/dev/sdb1", "wb");
    FILE* fp2 = fopen("/dev/sdc1", "wb");
    if(fp1 = NULL){
      printf("Error opening USB device /dev/sdb1\n");
      fp1_available = false;
    }


     // Open file pointer for USB device 2:
    if (fp2 == NULL) {
      printf("Error opening USB device /dev/sdc1\n");
      fp2_available = false;
    }
    
    if ((!fp1 && !fp2) || (fp1 && !fp2) || (!fp1 && fp2)) {
      // Error if both devices are not available, or if only one device is available
      printf("Error opening USB devices\n");
      return -1;
    }

    // Open local file for reading:
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

    printf("Request sent: %s\n", request);
    
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
      if (fp1_available) {  // write to the first USB device if it's available
        int bytes_written_1 = fwrite(file_data, 1, bytes_read, fp1);
        if (bytes_written_1 < 0) {
          printf("Can't write file data to fp1\n");
          return -1;
        }
        total_bytes += bytes_written_1;
      }

      if (fp2_available) {  // write to the second USB device if it's available
        int bytes_written_2 = fwrite(file_data, 1, bytes_read, fp2);
        if (bytes_written_2 < 0) {
          printf("Can't write file data to fp2\n");
          return -1;
        }else if (bytes_written_2 < bytes_read) {
          // if the second USB device doesn't have enough space, set the flag to false
          fp2_available = false;
          total_bytes += bytes_written_2;
          fseek(fp, total_bytes, SEEK_SET);  // set the file pointer to the position where the data wasn't written to the second USB device
          break;
        }
      }
    }

    if (!fp2_available) {
      fp2 = fopen("/dev/sdc1", "wb");
      if (!fp2) {
        printf("Can't open second USB device\n");
        return -1;
      }
      while ((bytes_read = fread(file_data, 1, sizeof(file_data), fp)) > 0) {
        int bytes_written_2 = fwrite(file_data, 1, bytes_read, fp2);
        if (bytes_written_2 < 0) {
          printf("Can't write file data to fp2\n");
          return -1;
        }
      }
      fp2_available = true;  // set the flag to true to indicate that the second USB device is available again
    }

    printf("%d bytes written to USB devices\n", total_bytes);
    
    // Close file and socket:
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    
    // Receive response from server:
    if (recv(socket_desc, client_message, 8196, 0) < 0){
      printf("Can't receive\n");
      return -1;
    }
    

    printf("File sent successfully\n");
    close(socket_desc);
    return 0;
    
  }
  else {
    sprintf(request, "%s %s %s", command, remote_file_path, local_file_path);
  }

  sprintf(request, "File %s written to USB devices\n", local_file_path);

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