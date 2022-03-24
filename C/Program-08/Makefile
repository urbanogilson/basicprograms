CC=gcc
CFLAGS=-Wall -Wextra # -Werror
DEPS = network.h
SERVER_OBJ = server.o network.o
CLIENT_OBJ = client.o network.o
LISTENER_OBJ = listener.o network.o
SHOWIP_OBJ = showip.o

all: server client showip listener

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(SERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(CLIENT_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

showip: $(SHOWIP_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

listener: $(LISTENER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm server client showip listener *.o