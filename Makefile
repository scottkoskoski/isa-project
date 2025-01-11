CC = gcc
CFLAGS = -Wall -Werror -Iinclude
SRC = src/machine.c src/fetch.c src/decode.c src/execute.c src/memory.c src/writeback.c src/alu.c main.c
OBJ = $(SRC:.c=.o)
TARGET = riscv_emulator

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
