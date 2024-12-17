CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lSDL3

TARGET = main

SRC = main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
