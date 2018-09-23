all:map_text_file1 map_text_file2

map_text_file1:map_text_file1.o
	gcc -Wall -o map_text_file1 map_text_file1.o

map_text_file1.o:map_text_file1.c
	gcc -Wall -c -g map_text_file1.c
	
map_text_file2:map_text_file2.o
	gcc -Wall -o map_text_file2 map_text_file2.o

map_text_file2.o:map_text_file2.c
	gcc -Wall -c -g map_text_file2.c
	
dep:
	gcc -M *.c > .depend
	
clean:
	rm -f map_text_file1 map_text_file2 *.o *~*.*~
	
