CC = gcc
CFLAGS = -Wall -lrt -g
DEPS = common.h types.h common_mac.h
OBJ = server.o common.o common_mac.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

client: client.o common.o common_mac.o
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o server client