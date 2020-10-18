# C Compiler
CC = clang

# Arguments for Compiler
ARGS = lpthread

server: server.o
	$(CC) -$(ARGS) bin/server.o -o bin/server

server.o:
	$(CC) -c src/Server.c -o bin/server.o

client: client.o
	$(CC) -$(ARGS)  bin/client.o -o bin/client

client.o:
	$(CC) -c src/Client.c -o bin/client.o

clean:
	rm -f bin/*

test:
	clang -$(ARGS) src/test.c -o bin/test && ./bin/test