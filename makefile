TARGET = tetris
SRC = tetris.c
CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS =

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)