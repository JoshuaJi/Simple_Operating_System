all: server client

server: print_server.c
	gcc print_server.c -o server -lrt -pthread
client: print_client.c
	gcc print_client.c -o client -lrt -pthread
