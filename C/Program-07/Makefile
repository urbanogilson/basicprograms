CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -lm -std=c11 -O -D_FORTIFY_SOURCE=2
LIBRARIES= -lm
DEPS = hash_table.h prime.h
OBJ = program-07.o hash_table.o prime.o
TEST = 
PROGRAM = program-07

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBRARIES) 

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES) 

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