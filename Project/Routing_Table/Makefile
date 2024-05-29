# Makefile for server and client

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Targets
TARGET = client server

# Default target
all: $(TARGET)

# Compile common object file
common.o: common.c common.h types.h
	$(CC) $(CFLAGS) -c common.c -o common.o


# Compile client
client: client.c common.h common.o
	$(CC) $(CFLAGS) client.c common.o -o client

# Compile server
server: server.c common.h common.o
	$(CC) $(CFLAGS) server.c common.o -o server

# Clean up
clean:
	rm -f $(TARGET) common.o