CC=gcc
CFLAG=-Wall -g

all: signal threads syscall

signal: signal.o
	$(CC) $(CFLAG) -m32 signal.o -o signal

signal.o: signal.c
	$(CC) $(CFLAG) -m32 -c signal.c

threads: threads.o
	$(CC) $(CFLAG) -m32 -pthread threads.o -o threads

threads.o: threads.c
	$(CC) $(CFLAG) -m32 -c threads.c

syscall: syscall.o
	$(CC) $(CFLAG) -m32 syscall.o -o syscall

syscall.o: syscall.c
	$(CC) $(CFLAG) -m32 -c syscall.c

clean:
	rm signal threads *.o