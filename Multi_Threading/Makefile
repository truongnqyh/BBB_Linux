
-include Test4/test4.mak

MODULE_SRCS = MutexWrite1File.c Write.c Write.h

.PHONY = all clean

CC = gcc			

LINKERFLAG = -lm

all: MutexWrite1File

MutexWrite1File: MutexWrite1File.o Write.o
	@echo "Checking.."s
	$(CC) -lm MutexWrite1File.o -o MutexWrite1File Write.o

MutexWrite1File.o: MutexWrite1File.c
	@echo "Creating object.."
	$(CC) -c $(MODULE_SRCS)

clean:
	@echo "Cleaning up..."
	rm -rvf MutexWrite1File.o MutexWrite1File

test: Vector3D.o test.o
    $(CPP) Vector3D.o test.o -o test

Vector3D/test.o: test.cpp
    $(CPP) -c test.cpp -o test.o $(CXXFLAGS)

Vector3D.o: Vector3D.cpp Vector3D.hpp
    $(CPP) -c Vector3D.cpp -o Vector3D.o $(CXXFLAGS)