all: philosopher

philosopher: philosopher.o
	g++ -Wall -o philosopher philosopher.o -lpthread

philosopher.o:
	g++ -Wall -c philosopher.cpp

dep:
	g++ -M *.cpp > .depend

clean:
	rm -f philosopher *.o *~ *.*~
