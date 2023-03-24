all: sender catcher

CC=gcc
FLAGS=-Wall -D DEBUG

sender: sender.c communicates.h
	$(CC) $(FLAGS) sender.c communicates.h -o sender.out

catcher: catcher.c communicates.h
	$(CC) $(FLAGS) catcher.c communicates.h -o catcher.out

clean:
	rm *.out