# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -g
LDFLAGS =

# Target executable files
SERVER = server
CLIENT = client

vpath %.o .
vpath %.c .

# Object files
SERVER_OBJ = server.o common.o
CLIENT_OBJ = client.o common.o

# Header files
HEADERS = types.h common.h

# Server target
.PHONY
server: $(SERVER_OBJ)
	$(CC) -o server $(SERVER_OBJ) $(CFLAGS) 

# Client target
$(CLIENT): $(CLIENT_OBJ) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up object files and executables
clean:
	rm -f $(SERVER) $(CLIENT) $(SERVER_OBJ) $(CLIENT_OBJ)

# Compile server.c to server.o
server.o: server.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

# Compile client.c to client.o
client.o: client.c $(HEADERS)
	$(	) $(CFLAGS) -c $<

# Compile common.c to common.o
common.o: common.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

print-%: ; $(error DEBUG Info: $* is $($*) ($(value $*)) (from $(origin $*)))