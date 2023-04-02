all: parallel_integration

CC=gcc
FLAGS=-Wall -O3

parallel_integration: parallel_integration.c safe_functions
	$(CC) $(FLAGS) -Wl,-rpath ./ safe_functions.h parallel_integration.c -o parallel_integration.out -L -./ libsafe_functions.so

safe_functions: safe_functions.c safe_functions.h
	$(CC) $(FLAGS) -shared -fPIC safe_functions.h safe_functions.c -o libsafe_functions.so

clean:
	-@rm 2> /dev/null *.out *.gch *.so
