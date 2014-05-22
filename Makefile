CC=g++
CFLAGS=-Wall


# dependencies of binary targets;
# explicitly included with binary compilation
DEPS_BIN=sort.o util.o stats.o

# dependencies of all object files;
# not explicity included with object compilation
DEPS_OBJ=types.hpp

# directory to place binary in
DIR_BIN=bin

all: test

test: test.o $(DEPS_BIN)
	mkdir -p $(DIR_BIN)
	$(CC) -o $(DIR_BIN)/test test.o $(DEPS_BIN)

%.o: %.cpp %.hpp $(DEPS_OBJ)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf $(DIR_BIN)
