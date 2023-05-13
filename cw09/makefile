all: main

CC = gcc
CFLAGS = -Wall -O3 -pthread

main: main.c
	$(CC) $(CFLAGS) -o main.out main.c

clean:
	-@rm -f main.out 2>/dev/null | true