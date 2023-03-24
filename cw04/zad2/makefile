all: signal_actions test clean

CC=gcc
FLAGS=-Wall

signal_actions: signal_actions.c
	$(CC) $(FLAGS) signal_actions.c -o signal_actions.out

test: signal_actions
	-@./signal_actions.out

clean:
	rm *.out