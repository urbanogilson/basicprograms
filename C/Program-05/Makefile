CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -lm
DEPS = matrix.h
OBJ = program-05.o matrix.o
PROGRAM = program-05

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBRARIES) 

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES) 

.PHONY: clean

clean:
	rm $(PROGRAM) *.o