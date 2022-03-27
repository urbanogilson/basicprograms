CC=gcc
CFLAGS=-Wall -Wextra # -Werror
DEPS = network.h
SERVER_OBJ = server.o network.o
CLIENT_OBJ = client.o network.o
LISTENER_OBJ = listener.o network.o
TALKER_OBJ = talker.o network.o
SHOWIP_OBJ = showip.o
POLL_OBJ = poll.o
POLLSERVER_OBJ = pollserver.o network.o
SELECT_OBJ = select.o
SELECTSERVER_OBJ = selectserver.o  network.o
BROADCASTER_OBJ = broadcaster.o

all: server client showip listener talker poll pollserver select selectserver broadcaster

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

talker: $(TALKER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

poll: $(POLL_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

pollserver: $(POLLSERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

select: $(SELECT_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

selectserver: $(SELECTSERVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

broadcaster: $(BROADCASTER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm server client showip listener talker poll pollserver select selectserver broadcaster *.o