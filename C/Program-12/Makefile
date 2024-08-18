CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -g -static -static-libgcc

TARGET = program-12

SRC = program-12.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
