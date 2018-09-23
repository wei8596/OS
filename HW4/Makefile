all: _test

_test: mm.o test.o
	gcc -Wall -o _test mm.o test.o
	
mm.o:
	gcc -Wall -c mm.c

test.o:
	gcc -Wall -c test.c
	
dep:
	gcc -M *.c > .depend

clean:
	rm -f mm _test test *.o *~*.*~
