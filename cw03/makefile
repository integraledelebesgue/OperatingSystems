all: zad1.out zad2.out zad3.out

CC=gcc
FLAGS=-Wall -O3

zad1.out: zad1/spawn.c
	$(CC) $(FLAGS) zad1/spawn.c -o zad1/spawn.out

zad2.out: zad2/exec_ls.c
	$(CC) $(FLAGS) zad2/exec_ls.c -o zad2/exec_ls.out

zad3.out: zad3/string_search.c # Wersja programu z dodatkową instrukcją wait - ładniejsze wyjście
	$(CC) $(FLAGS) zad3/string_search.c -o zad3/string_search.out -D SYNCHRONIZE

zad3_nosync: # Wersja programu bez instrukcji wait, w zasadzie zgodna  z wymaganiami
	$(CC) $(FLAGS) zad3/string_search.c -o zad3/string_search_nosync.out

clean:
	-@find ./ -name "*.out" -type f -delete 2> /dev/null
