CC=gcc
CFLAGS= -save-temps -fverbose-asm -masm=att
DEPS = bracket.h
OBJ = program-06.o sum.o
PROGRAM = program-06

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBRARIES) 

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES) 

.PHONY: clean

clean:
	rm -r $(PROGRAM) *.o