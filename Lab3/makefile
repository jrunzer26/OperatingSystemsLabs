CC = gcc
CFLAGS = -Wall -Wextra -std=c99 
LFLAGS = 
LIBS = -lm -lpthread
SOURCES = sodoku.c
OBJECTS = $(subst .c,.o,$(SOURCES))
EXE = sodoku
.PHONY: clean help

sodoku : sodoku.c
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

all : $(EXE)

clean:
	rm -f $(OBJECTS) $(EXE) *~

cleanup:
	rm -f $(OBJECTS) *~

help:
	@echo "Valid targets:"
	@echo "  all:    generates all binary files"
	@echo "  clean:  removes .o and .exe files"