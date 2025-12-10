# Project name
OUT := noto

SRC_DIR := source
INC_DIR := include

# Find sources
ASM_SRC := $(shell find $(SRC_DIR) -type f -name '*.asm')
C_SRC   := $(shell find $(SRC_DIR) -type f -name '*.c')

# Derive object files
OBJ := $(ASM_SRC:.asm=.o) $(C_SRC:.c=.o)

# Tools
CC := gcc
AS := nasm

CFLAGS := -g -O0 -I$(INC_DIR) -std=c99 -finstrument-functions
ASFLAGS := -g -F dwarf -f elf64 -I$(SRC_DIR)/asm
LDFLAGS := -g -O0 -no-pie -lncursesw

.PHONY: make clean

make: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

# Compile C
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM
$(SRC_DIR)/%.o: $(SRC_DIR)/%.asm
	$(AS) $(ASFLAGS) $< -o $@


clean:
	find . -type f -name '*.o' -delete
	rm -f noto