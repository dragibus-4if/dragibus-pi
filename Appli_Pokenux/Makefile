all: pokapt

pokapt: main.o
	gcc -o pokapt main.o

main.o:	main.c
	gcc -o main.o -c main.c -g -W -Wall -ansi -pedantic -std=c99

clean:
	rm -rf *.o
