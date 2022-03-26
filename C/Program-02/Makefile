CC=gcc
CFLAGS= -Wall -Wextra -Werror
OBJ = program-02.o
PROGRAM = program-02

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBRARIES) 

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES) 

.PHONY: clean

clean:
	rm $(PROGRAM) *.o