CC = gcc
CFLAGS = -std=c11 -Wall -D_DEFAULT_SOURCE -Iinclude -g -O0
LDFLAGS = -lm -lSDL2

SRCDIR = $(shell basename $(shell pwd))
DESTDIR ?= 
PREFIX ?= /usr

SRC0 =  src/raycaster.c src/main.c
OBJ0 = $(SRC0:%.c=%.c.o)
EXE0 = raycast

all: $(EXE0)

$(EXE0): $(OBJ0)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ0) $(EXE0)

install:
	cp $(EXE0) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXE0)

dist:
	cd .. && tar cvzf $(SRCDIR).tgz ./$(SRCDIR)

