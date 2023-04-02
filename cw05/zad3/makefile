all: integral

CC=gcc
FLAGS=-Wall

integral: integral.c component safe_functions
	$(CC) $(FLAGS) -Wl,-rpath ./ integral.c -L ./ libsafe_functions.so ../zad2/safe_functions.h -o integral.out

component: component.c safe_functions
	$(CC) $(FLAGS) -Wl,-rpath ./ component.c -L ./ libsafe_functions.so ../zad2/safe_functions.h -o component.out

safe_functions:
	make -C ../zad2 safe_functions
	cp ../zad2/libsafe_functions.so .
	make -C ../zad2 clean

clean:
	-@rm *.out *.so
