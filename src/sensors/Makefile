ROOT_DIR := ../

all: libsensorblast.a

SOURCES := hdc2010.c interface/sb_i2c_inf.c sb_logger.c
OBJECTS := hdc2010.o interface/sb_i2c_inf.o sb_logger.o

CFLAGS := -c -g -I$(ROOT_DIR)/include/ -I./interface/

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

libsensorblast.a: $(OBJECTS)
	ar rc $@ $^

clean:
	rm -rf *.o
	rm -rf *.a
	rm -rf interface/*.o


