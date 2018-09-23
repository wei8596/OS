all: philosopher

philosopher: philosopher.o
	gcc -Wall -o philosopher philosopher.o -lpthread

philosopher.o:
	gcc -Wall -c philosopher.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f philosopher *.o *~ *.*~
