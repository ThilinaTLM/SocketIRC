# C Compiler
CC = clang

# Arguments for Compiler
ARGS = lpthread

all: server client src/lib/cpool.h src/lib/cprofile.h src/lib/cmd.h src/lib/console.h

server: server.o
	$(CC) -$(ARGS) bin/server.o -o bin/server

server.o: src/lib/cpool.h src/lib/cprofile.h src/lib/cmd.h
	$(CC) -c src/Server.c -o bin/server.o

client: client.o
	$(CC) -$(ARGS)  bin/client.o -o bin/client

client.o: src/lib/cmd.h src/lib/console.h src/lib/cprofile.h 
	$(CC) -c src/Client.c -o bin/client.o

clean:
	rm -f bin/*
