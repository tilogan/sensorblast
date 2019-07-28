SENSOR_BLAST_ROOT_DIR := ../../../
AWS_SDK_DIR := ../../../../aws-iot-device-sdk-embedded-C/

CC := gcc

all: hdc2010_aws_publish

# Sensor Blast Soruces
ALL_SOURCES := $(shell find . -name '*.c')
INCLUDE_PATHS := -I$(SENSOR_BLAST_ROOT_DIR)src/sensors/
INCLUDE_PATHS += -I$(SENSOR_BLAST_ROOT_DIR)src/include/
INCLUDE_PATHS += -I$(shell pwd)
CFLAGS := -c -g
LDFLAGS := -L$(SENSOR_BLAST_ROOT_DIR)src/sensors/ -lsensorblast -lpthread

#IoT client directory
IOT_CLIENT_DIR = $(AWS_SDK_DIR)

PLATFORM_DIR = $(IOT_CLIENT_DIR)/platform/linux/mbedtls
PLATFORM_COMMON_DIR = $(IOT_CLIENT_DIR)/platform/linux/common

IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/include
IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/external_libs/jsmn
IOT_INCLUDE_DIRS += -I $(PLATFORM_COMMON_DIR)
IOT_INCLUDE_DIRS += -I $(PLATFORM_DIR)

IOT_SRC_FILES += $(shell find $(IOT_CLIENT_DIR)/src/ -name '*.c')
IOT_SRC_FILES += $(shell find $(IOT_CLIENT_DIR)/external_libs/jsmn -name '*.c')
IOT_SRC_FILES += $(shell find $(PLATFORM_DIR)/ -name '*.c')
IOT_SRC_FILES += $(shell find $(PLATFORM_COMMON_DIR)/ -name '*.c')

ALL_SOURCES += $(IOT_SRC_FILES)
CFLAGS += $(IOT_INCLUDE_DIRS)
CFLAGS += $(INCLUDE_PATHS)
ALL_OBJECTS := $(ALL_SOURCES:.c=.o) 

#TLS - mbedtls
MBEDTLS_DIR = $(IOT_CLIENT_DIR)/external_libs/mbedTLS
TLS_LIB_DIR = $(MBEDTLS_DIR)/library
TLS_INCLUDE_DIR = -I $(MBEDTLS_DIR)/include
EXTERNAL_LIBS += -L$(TLS_LIB_DIR)
LDFLAGS += -Wl,-rpath,$(TLS_LIB_DIR)
LDFLAGS += -ldl $(TLS_LIB_DIR)/libmbedtls.a $(TLS_LIB_DIR)/libmbedcrypto.a $(TLS_LIB_DIR)/libmbedx509.a -lpthread
CFLAGS += $(TLS_INCLUDE_DIR)


%.o : %.c
	@echo CC $^
	@$(CC) $(CFLAGS) $^ -o $@

hdc2010_aws_publish: $(ALL_OBJECTS)
	@echo LD $@
	@$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf *.o
	rm -rf hdc2010_aws_publish
