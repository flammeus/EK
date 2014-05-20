TARGET = test
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=.o)

.PHONY : clean

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c99 -g

all: $(TARGET)

$(TARGET): $(SOURCES) $(OBJECTS)
	$(CC) $(CFLAGS) -c $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS)
