# C Compiler
CC = clang

# Arguments for Compiler
ARGS = lpthread

server: server.o
	$(CC) -$(ARGS) bin/server.o -o bin/server

server.o:
	$(CC) -c src/Server.c -o bin/server.o

client: client.o
	$(CC) bin/client.o -o bin/client

client.o:
	$(CC) -c src/Client.c -o bin/client.o

clean:
	rm -f bin/*

test:
	clang src/test.c -o bin/test && ./bin/test