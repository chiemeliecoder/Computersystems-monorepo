all: server fgets

server: server.c
	
	gcc -Wall server.c -o server -lpthread

client: fgets.c
	gcc fgets.c -o fgets -pthread
