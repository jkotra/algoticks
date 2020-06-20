CC=gcc
IDIR = include/
CFLAGS=-I$(IDIR) -Wall -Wl,-rpath,bin/algorithms/
LINKS = -ljson-c -ldl
DEBUG_SYMBOLS = -g
SRCFILES = $(wildcard src/*.c)
ALGORITHMS = $(wildcard src/algorithms/*.c)
ALGOS = src/algorithms/3Greens.c


DEBUG_BUILD_PATH = bin/debug
RELEASE_BUILD_PATH = bin/release
ALGORITHMS_BUILD_PATH = bin/algorithms

all:
	mkdir -p bin/algorithms/

release: $(SRCFILES)
	$(CC) $(CFLAGS) $(LINKS) $(SRCFILES) -o $(RELEASE_BUILD_PATH)

debug: $(SRCFILES)
	$(CC) $(CFLAGS) $(DEBUG_SYMBOLS) $(LINKS) $(SRCFILES) -o $(DEBUG_BUILD_PATH)

algorithms: $(ALGORITHMS)
	$(foreach file, $(ALGORITHMS), gcc -shared -fPIC $(file) -o $(patsubst %.c,%.so,$(file)); mv $(patsubst %.c,%.so,$(file)) bin/algorithms/)

clean:
	rm bin/algorithms/*
	rm bin/debug
	rm bin/release