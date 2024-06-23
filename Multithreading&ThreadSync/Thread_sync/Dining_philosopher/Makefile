CC = gcc
CFLAGS = -Wall -lrt -g -lpthread
DEPS = dnphil.h
OBJ = dnphil.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dnphil: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o dnphil