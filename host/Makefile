
# # Compiler
# CC = gcc

# # Compiler flags
# CFLAGS = -Wall -g

# # Executable name
# TARGET = hello

# # Source file
# SRC = main.c

# # Object file
# OBJ = main.o

# # Default target 
# # to create $(TARGET), we need $(OBJ)
# $(TARGET): $(OBJ)
# 	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# # Compile C file into object file 
# %.o: %.c 
# 	$(CC) $(CFLAGS) -c $< -o $@

# run:
# 	./$(TARGET)

# # Clean up generarted files 
# clean:
# 	rm -f $(OBJ) $(TARGET)

CC      ?= $(CROSS_COMPILE)gcc
LD      ?= $(CROSS_COMPILE)ld
AR      ?= $(CROSS_COMPILE)ar
NM      ?= $(CROSS_COMPILE)nm
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
READELF ?= $(CROSS_COMPILE)readelf

OBJS = main.o

CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include -I./include
#Add/link other required libraries here
LDADD += -lteec -L$(TEEC_EXPORT)/lib

BINARY = optee_example_digital_wallet

.PHONY: all
all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $< $(LDADD)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
