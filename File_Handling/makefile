.PHONY = all clean

CC = gcc			# compiler to use

LINKERFLAG = -lm

SRCS := foo.c
BINS := foo

all: foo

foo: foo.o
	@echo "Checking.."
	gcc -lm foo.o -o foo

foo.o: foo.c
	@echo "Creating object.."
	gcc -c foo.c

clean:
	@echo "Cleaning up..."
	rm -rvf foo.o foo