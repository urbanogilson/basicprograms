CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c17 -O -D_FORTIFY_SOURCE=2
DEPS = linked-list.h
OBJ = program-03.o linked-list.o
TEST = tests.c linked-list.c
PROGRAM = program-03

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f tests $(PROGRAM) valgrind.txt *.o *~ core $(INCDIR)/*~ 

run:
	./$(PROGRAM)

valgrind:
	valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--verbose \
		--log-file=valgrind.txt \
		./$(PROGRAM)

tests:
	$(CC) -o tests $(TEST) $(CFLAGS) && valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--verbose \
		--log-file=tests.txt \
		./tests \
		&& rm -f tests