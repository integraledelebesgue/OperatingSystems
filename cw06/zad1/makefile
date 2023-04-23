all: clean server client

CC = gcc
CFLAGS = -Wall -pthread

server: safe_functions.o client_storage.o message_processing.o logs.o
	$(CC) $(CFLAGS) safe_functions.h messages.h safe_functions.o client_storage.o message_processing.o logs.o server.c -o server.out

client: safe_functions.o
	$(CC) $(CFLAGS) safe_functions.h messages.h safe_functions.o client.c -o client.out

safe_functions.o:
	$(CC) $(CFLAGS) -c safe_functions.c

client_storage.o: safe_functions.o
	$(CC) $(CFLAGS) -c messages.h safe_functions.h client_storage.c

message_processing.o: safe_functions.o client_storage.o
	$(CC) $(CFLAGS) -c messages.h safe_functions.h message_processing.c

logs.o: messages.h client_storage.o
	$(CC) $(CFLAGS) -c messages.h logs.c

clean:
	-@rm *.o *.out *.gch