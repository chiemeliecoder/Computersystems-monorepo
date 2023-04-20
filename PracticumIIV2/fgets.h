#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define MAXFILE 100
#define FILENAME 100
#define SIZE 1024

#ifndef FGETS_H
#define FGETS_H
#endif


#define MAX_FILENAME_LEN 100
#define MAX_FILE_SIZE 8192
#define MAX_FILENAME_LEN2 256
#define MAX_COMMAND_LEN 256
