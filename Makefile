CC=gcc
IDIR = include/
CFLAGS=-I$(IDIR) -Wall -Wl,-rpath,bin/algorithms/
LINKS = -ljson-c -ldl
DEBUG_SYMBOLS = -g
MODULES = src/main.c src/debug.c src/misc.c src/csvutils.c src/sim.c src/dashboard.c src/timeutils.c
ALGOS = src/algorithms/3Greens.c


DEBUG_BUILD_PATH = bin/debug
RELEASE_BUILD_PATH = bin/release

debug: src/main.c
	$(CC) $(CFLAGS) $(DEBUG_SYMBOLS) $(LINKS) $(MODULES) $(ALGOS) -o $(DEBUG_BUILD_PATH)
	gcc -shared -fPIC src/algorithms/3Greens.c -o bin/algorithms/3Greens.so