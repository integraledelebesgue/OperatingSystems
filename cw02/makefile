all: source test_all clean

CC=gcc
FLAGS=-Wall -O3

source: zad1_out zad2_out zad3_out zad4_out


zad1_out: zad1/char_replacement.c
	$(CC) $(FLAGS) zad1/char_replacement.c -o zad1/zad1_lib.out	
	$(CC) $(FLAGS) zad1/char_replacement.c -D SYS -o zad1/zad1_sys.out


zad2_out: zad2/reverse_copy.c
	$(CC) $(FLAGS) zad2/reverse_copy.c -D BUFFER_SIZE=1024 -o zad2/zad2_1024.out
	$(CC) $(FLAGS) zad2/reverse_copy.c -o zad2/zad2_1.out


zad3_out: zad3/list_dir.c
	$(CC) $(FLAGS) zad3/list_dir.c -o zad3/zad3.out


zad4_out: zad4/list_rec.c
	$(CC) $(FLAGS) zad4/list_rec.c -o zad4/zad4.out



test_all: test_zad1 test_zad2 test_zad3 test_zad4


test_zad1: zad1_out test_out
	tr a x < test/pan-tadeusz.txt > test/pan-tadeusz-tr.txt
	
	echo "Lib:" > zad1/pomiar_zad_1.txt
	test/test.out "zad1/zad1_lib.out" 4 "a" "x" "test/pan-tadeusz.txt" "test/pan-tadeusz-replaced.txt" >> zad1/pomiar_zad_1.txt
	-diff -s test/pan-tadeusz-replaced.txt test/pan-tadeusz-tr.txt >> zad1/pomiar_zad_1.txt

	echo "Sys:" >> zad1/pomiar_zad_1.txt
	test/test.out "zad1/zad1_sys.out" 4 "a" "x" "test/pan-tadeusz.txt" "test/pan-tadeusz-replaced.txt" >> zad1/pomiar_zad_1.txt
	-diff -s test/pan-tadeusz-replaced.txt test/pan-tadeusz-tr.txt >> zad1/pomiar_zad_1.txt
	

test_zad2: zad2_out test_out
	echo '\n' > test/pan-tadeusz-reversed-tac.txt
	tac < test/pan-tadeusz.txt | rev >> test/pan-tadeusz-reversed-tac.txt

	echo "Buffer 1 byte:" > zad2/pomiar_zad_2.txt
	test/test.out "zad2/zad2_1.out" 2 "test/pan-tadeusz.txt" "test/pan-tadeusz-reversed.txt" >> zad2/pomiar_zad_2.txt
	-diff -s test/pan-tadeusz-reversed.txt test/pan-tadeusz-reversed-tac.txt >> zad2/pomiar_zad_2.txt

	echo "Buffer 1024 bytes:" >> zad2/pomiar_zad_2.txt
	test/test.out "zad2/zad2_1024.out" 2 "test/pan-tadeusz.txt" "test/pan-tadeusz-reversed.txt" >> zad2/pomiar_zad_2.txt
	-diff -s test/pan-tadeusz-reversed.txt test/pan-tadeusz-reversed-tac.txt >> zad2/pomiar_zad_2.txt


test_zad3: zad3_out test_out
	test/test.out zad3/zad3.out 1 ./ > zad3/pomiar_zad_3.txt
	echo '\n' >> zad3/pomiar_zad_3.txt
	echo "Wywołanie wc -c *:" >> zad3/pomiar_zad_3.txt
	-wc -c * >> zad3/pomiar_zad_3.txt
	

test_zad4: zad4_out test_out
	test/test.out zad4/zad4.out 1 ./ > zad4/pomiar_zad_4.txt
	echo '\n' >> zad4/pomiar_zad_4.txt
	echo "Wywołanie find ./ | xargs wc --bytes:" >> zad4/pomiar_zad_4.txt
	-find ./ | xargs wc --bytes >> zad4/pomiar_zad_4.txt


test_out: test/test.c
	$(CC) test/test.c -o test/test.out


clean_files:
	rm test/pan-tadeusz-*.txt

clean:
	find ./ -name "*.out" -type f -delete
