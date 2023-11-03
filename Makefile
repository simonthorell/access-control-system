PROG = main.exe
# LÄGG TILL ALLA FILER I SRC
SRC = main.c safeinput.c
# EXTRA VARNINGAR I CFLAGS	
CFLAGS = -g -Wall -Wextra -Wundef -pedantic -Werror
LIBS = 

all: $(PROG)

$(PROG): $(SRC)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(SRC)  $(LIBS) 

clean:
	rm -f $(PROG)

.PHONY: all clean