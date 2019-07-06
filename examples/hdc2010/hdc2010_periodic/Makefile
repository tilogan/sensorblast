ROOT_DIR := ../../../

all: hdc2010_periodic

SOURCES := hdc2010_periodic.c
OBJECTS := hdc2010_periodic.o

CC := gcc

CFLAGS := -c -g -I$(ROOT_DIR)src/sensors/ -I$(ROOT_DIR)src/include/
LDFLAGS := -L$(ROOT_DIR)src/sensors/ -lsensorblast

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

hdc2010_periodic: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf *.o
	rm -rf hdc2010_periodic
