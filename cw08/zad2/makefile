all: main

CC = gcc
CFLAGS = -Wpedantic -O3
LINKS = -lpthread -lncursesw

main: main.c
	$(CC) $(CFLAGS) -o life.out main.c $(LINKS) -D GRID_SIZE=50

clean:
	-@rm -f *.out 2>/dev/null || true
	