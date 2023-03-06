all: zad1_static zad1_shared zad2_static zad2_shared zad2_dl static_test shared_test dl_test clean

OPTIMIZATION=O3
CC=gcc
FLAGS=-Wall

zad1_static: filenames.o memory_blocks.o
	ar rcs lib_zad1.a filenames.o memory_blocks.o zad1/errors.h
	cp lib_zad1.a zad1
	cp lib_zad1.a zad2

zad1_shared: filenames_pic.o memory_blocks_pic.o
	$(CC) -Wall -$(OPTIMIZATION) -shared filenames_pic.o memory_blocks_pic.o -o lib_zad1.so.1
	cp lib_zad1.so.1 zad1
	cp lib_zad1.so.1 zad2
	cp lib_zad1.so.1 zad3

filenames.o: zad1/filenames.c
	$(CC) $(FLAGS) -$(OPTIMIZATION) -c zad1/filenames.c

filenames_pic.o: zad1/filenames.c
	$(CC) $(FLAGS) -$(OPTIMIZATION) -fPIC -c zad1/filenames.c -o filenames_pic.o

memory_blocks.o: zad1/memory_blocks.c
	$(CC) $(FLAGS) -$(OPTIMIZATION) -c zad1/memory_blocks.c zad1/filenames.c zad1/errors.h

memory_blocks_pic.o: zad1/memory_blocks.c
	$(CC) $(FLAGS) -$(OPTIMIZATION) -fPIC -c zad1/memory_blocks.c -o memory_blocks_pic.o

zad2_static: zad1_static repl.o command_processing.o
	$(CC) $(FLAGS) -$(OPTIMIZATION) repl.o command_processing.o lib_zad1.a  -o zad2_static.out
	mv zad2_static.out zad2

zad2_shared: zad1_shared repl.o command_processing.o
	$(CC) $(FLAGS) -$(OPTIMIZATION) -Wl,-R -Wl,./ repl.o command_processing.o lib_zad1.so.1 -o zad2_shared.out
	mv zad2_shared.out zad2

zad2_dl: zad1_shared repl_dl.o command_processing.o
	$(CC) $(FLAGS) -$(OPTIMIZATION) -Wl,-R -Wl,./ repl_dl.o command_processing.o lib_zad1.so.1 -o zad2_dl.out -ldl
	mv zad2_dl.out zad2

repl.o: zad2/repl.c
	$(CC) $(FLAGS) -$(OPTIMIZATION) -c zad2/repl.c

repl_dl.o: zad2/repl_dl.c
	$(CC) $(FLAGS) -$(OPTIMIZATION) -c zad2/repl_dl.c

command_processing.o: zad2/command_processing.c
	$(CC) $(FLAGS) -$(OPTIMIZATION) -c zad2/command_processing.c zad1/errors.h zad2/repl.h

static_test: zad2_static test_executable
	zad3/test.out zad2/zad2_static.out > zad3/result_static_$(OPTIMIZATION).txt

shared_test: zad2_shared test_executable
	zad3/test.out zad2/zad2_shared.out > zad3/result_shared_$(OPTIMIZATION).txt

dl_test: zad2_dl test_executable
	zad3/test.out zad2/zad2_dl.out > zad3/result_dl_$(OPTIMIZATION).txt

test_executable:
	$(CC) $(FLAGS) -$(OPTIMIZATION) zad3/test.c -o zad3/test.out

clean:
	-@rm *.o 2> /dev/null
	-@rm ./lib_zad1.a 2> /dev/null
	-@rm ./lib_zad1.so.1 2> /dev/null

clean_binaries:
	find ./ -name "*.out" -type f -delete
	find ./ -name "*.gch" -type f -delete
	find ./ -name "*.so*" -type f -delete
	find ./ -name "*.a" -type f -delete
