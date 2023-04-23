all: server barber customer

CC = gcc
CFLAGS = -Wall -pthread -lrt

server: server.c safe_functions.o
	$(CC) $(CFLAGS) -o server.out common.h safe_functions.h safe_functions.o server.c

barber: barber.c safe_functions.o
	$(CC) $(CFLAGS) -o barber.out common.h safe_functions.h safe_functions.o barber.c

customer: customer.c safe_functions.o
	$(CC) $(CFLAGS) -o customer.out common.h safe_functions.h safe_functions.o customer.c

safe_functions.o: safe_functions.c safe_functions.h
	$(CC) $(CFLAGS) -c safe_functions.c

clean:
	rm *.out