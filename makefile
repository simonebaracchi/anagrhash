CC = gcc
SOURCES = anagram.c result.c main.c list.c
FLAGS = -Wall `pkg-config --cflags --libs glib-2.0` -lgthread-2.0 -lmhash -lpthread -lrt

all:
	$(CC) $(SOURCES) -o anagrhash $(FLAGS)
clean:
	rm -f anagrhash
