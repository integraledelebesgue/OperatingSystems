all: fork_sig test clean

CC=gcc
FLAGS=-Wall

fork_sig: other fork_sig.c
	$(CC) $(FLAGS) fork_sig.c -o fork_sig.out

other:	
	$(CC) $(FLAGS) other.c -o other.out

test: fork_sig
	./fork_sig.out ignore
	-@./fork_sig.out handler
	./fork_sig.out mask
	./fork_sig.out pending

clean:
	-@rm *.out