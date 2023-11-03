# Directories
INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj

# Compiler
CC = gcc

# Flags
CFLAGS = -g -Wall -Wextra -Wundef -pedantic -Werror -I$(INC_DIR)

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Program name
PROG = main.exe

# Default rule
all: $(PROG)

# Linking rule
$(PROG): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

# Compilation rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ_DIR)/*.o $(PROG)

.PHONY: all clean