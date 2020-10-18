INC_DIR = src/lib
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

CC = clang # C Compiler
CFLAGS = -lpthread # compiler flags

SRCS = $(SRC_DIR)/Client.c $(SRC_DIR)/Server.c
OBJS = $(OBJ_DIR)/client.o $(OBJ_DIR)/server.o
BINS = $(BIN_DIR)/client $(BIN_DIR)/server

DEPS = $(INC_DIR)/cmd.h $(INC_DIR)/console.h $(INC_DIR)/cpool.h $(INC_DIR)/cprofile.h

all: $(BINS)

$(BIN_DIR)/client: $(OBJ_DIR)/client.o
	$(CC) $(CFLAGS) $(OBJ_DIR)/client.o -o $(BIN_DIR)/client

$(BIN_DIR)/server: $(OBJ_DIR)/server.o
	$(CC) $(CFLAGS) $(OBJ_DIR)/server.o -o $(BIN_DIR)/server

$(OBJ_DIR)/client.o: $(SRC_DIR)/Client.c
	$(CC) -c $(SRC_DIR)/Client.c -o $(OBJ_DIR)/client.o

$(OBJ_DIR)/server.o: $(SRC_DIR)/Server.c
	$(CC) -c $(SRC_DIR)/Server.c -o $(OBJ_DIR)/server.o

$(SRC_DIR)/%.c: $(DEPS)

clean:
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/*
