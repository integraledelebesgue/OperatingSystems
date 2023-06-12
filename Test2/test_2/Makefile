CC = gcc
CFLAGS = -Wall

all: test
	
main: main.c checks.h
	$(CC) $(CFLAGS) main.c -o main

test: main
	rm -f results.txt
	./main | tee -a results.txt
	
clean:
	rm -f main
