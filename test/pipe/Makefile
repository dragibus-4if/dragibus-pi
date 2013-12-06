CFLAGS=-pedantic -Wall -Wextra -Wfloat-equal -Wwrite-strings -Wpointer-arith \
	-Wcast-align -Wconversion -Wredundant-decls -Winit-self -Wswitch-default \
	-Wswitch-enum -Wundef -Winline -W -Wunused-parameter -ansi -Wshadow \
	-std=gnu99 -g -nostdlib

LDFLAGS=-g

all: a.out

a.out: main.o pipe.o mutex.o malloc.o
	gcc $^ $(LDFLAGS) -o $@

main.o: main.c pipe.h

malloc.o: malloc.c malloc.h

pipe.o: pipe.c pipe.h mutex.h

mutex.o: mutex.c mutex.h

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@
