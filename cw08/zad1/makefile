all: main

CC = gcc
CFLAGS = -Wpedantic -O3
LINKS = -lpthread -lncursesw

main: main.c
	$(CC) $(CFLAGS) main.c $(LINKS) -D GRID_SIZE=30 -o life.out

clean:
	-@rm -f *.out 2>/dev/null || true
