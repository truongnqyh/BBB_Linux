
# MODULE_SRCS := MutexWrite1File.c Write.c Write.h

# PROJECT_DIR := .
# .PHONY = all clean

# CC = gcc			

# all: MutexWrite1File

# MutexWrite1File: MutexWrite1File.o Write.o
# 	@echo "Checking.."s
# 	$(CC) MutexWrite1File.o Write.o -o MutexWrite1File

# MutexWrite1File.o/Write.o: MutexWrite1File.c Write.c
# 	@echo "Creating object.."
# 	$(CC) -c $(MODULE_SRCS) -I/home/truong/workspace/Linux_Aphu/BBB_Linux/Multi_Threading/Test4

# clean:
# 	@echo "Cleaning up..."
# 	rm -rvf MutexWrite1File.o MutexWrite1File

IDIR = .
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR = .

LIBS=-lm

_DEPS = Write.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = MutexWrite1File.o Write.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

MutexWrite1File: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 