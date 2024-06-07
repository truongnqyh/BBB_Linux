CC = gcc
CFLAGS = -Wall -lrt -g -lpthread
OBJ = helloworld.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

helloworld: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o