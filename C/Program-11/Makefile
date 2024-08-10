CC=/usr/bin/gcc
CFLAGS= -Wall -Werror -Wextra -lSDL2
INSTALL= sudo apt install
OBJ = program-11.o
PROGRAM = program-11
FS=fs

ifneq ($(strip $(RELEASE)),)
	CFLAGS += -O2
else ifneq ($(strip $(SANITIZE)),)
	CFLAGS += -fsanitize=address
else # DEBUG
	CFLAGS += -g3 -ggdb
endif

%.o: %.c %.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBRARIES)

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES) 

.PHONY: clean

clean:
	rm $(PROGRAM) *.o
