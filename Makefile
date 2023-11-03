PROG = main.exe
CC = gcc
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Retrieve a list of source files and replace the extension .c with .o for object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS = -g -Wall -Wextra -Wundef -pedantic -Werror -I$(INC_DIR)
LIBS = 

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^  $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROG) $(OBJ)

.PHONY: all clean