all: server client unlink_mem

server: print_server.c
	gcc print_server.c -o server -lrt -pthread
client: print_client.c
	gcc print_client.c -o client -lrt -pthread
unlink_mem: print_unlink.c
	gcc print_unlink.c -o unlink_mem -lrt
