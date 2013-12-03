CC = $(shell which gcc)
CFLAGS = $(CFLAGS_DEBUG) \
	 -pedantic -Wall -Wextra -Wfloat-equal -Wwrite-strings -Wpointer-arith \
	 -Wcast-align -Wconversion -Wredundant-decls -Winit-self \
	 -Wswitch-default -Wswitch-enum -Wundef -Winline -W -Wunused-parameter \
	 -ansi -Wshadow -std=c99
LDFLAGS =

CFLAGS_DEBUG = -g
LDFLAGS_DEBUG =

EXE = a.out

.PHONY: all clean

all: $(EXE)

$(EXE): main.o pipe.o mutex.o malloc.o
	gcc $^ $(LDFLAGS) -o $@

main.o: main.c pipe.h

malloc.o: malloc.c malloc.h

pipe.o: pipe.c pipe.h mutex.h

mutex.o: mutex.c mutex.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f *.o
	@rm $(EXE)
