CC=gcc
CFLAGS=-Wall -Wextra # -Werror
DEPS = network.h
SERVER_OBJ = server.o network.o
CLIENT_OBJ = client.o network.o
SHOWIP_OBJ = showip.o

all: server client showip

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(SERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(CLIENT_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

showip: $(SHOWIP_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm server client showip *.o