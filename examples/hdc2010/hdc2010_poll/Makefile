ROOT_DIR := ../../../

all: hdc2010_poll

SOURCES := hdc2010_poll.c
OBJECTS := hdc2010_poll.o

CC := gcc

CFLAGS := -c -g -I$(ROOT_DIR)src/sensors/ -I$(ROOT_DIR)src/include/
LDFLAGS := -L$(ROOT_DIR)src/sensors/ -lsensorblast -lpthread

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

hdc2010_poll: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf *.o
	rm -rf hdc2010_poll
