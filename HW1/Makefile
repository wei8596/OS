all: shell

shell: shell.o
	gcc -Wall -o shell shell.o

shell.o:
	gcc -Wall -c shell.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f shell *.o *~ *.*~
