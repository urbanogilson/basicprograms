CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c17 -O -D_FORTIFY_SOURCE=2
DEPS = linked-list.h
OBJ = program-03.o linked-list.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

program-03: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core $(INCDIR)/*~ 

run:
	./program-03