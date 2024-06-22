CC = gcc
CFLAGS = -Wall -lrt -g -lpthread
DEPS = queue.h
OBJ = pro_con.o queue.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

procon: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o server client