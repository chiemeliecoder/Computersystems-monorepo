#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define CMD_SIZE 100
#define MAXFILE 100
#define FILENAME 100
#define NUM_USB_DEVICES 2
#define SIZE 1024
#define BUFFER_SIZE 8196


#ifndef SERVER_H
#define SERVER_H
#endif


void perform_info_command(int client_sock, const char* remote_file_path);
