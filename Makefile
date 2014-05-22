CC=g++
CFLAGS=-Wall

LIBS=-ltiff

# dependencies of binary targets;
# explicitly included with binary compilation
DEPS_BIN=sort.o util.o stats.o data.o

# dependencies of all object files;
# not explicity included with object compilation
DEPS_OBJ=types.hpp

# directory to place binary in
DIR_BIN=bin

all: test landsat

landsat: landsat.o $(DEPS_BIN)
	mkdir -p $(DIR_BIN)
	$(CC) -o $(DIR_BIN)/landsat landsat.o $(DEPS_BIN) $(LIBS)

test: test.o $(DEPS_BIN)
	mkdir -p $(DIR_BIN)
	$(CC) -o $(DIR_BIN)/test test.o $(DEPS_BIN) $(LIBS)

%.o: %.cpp %.hpp $(DEPS_OBJ)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf $(DIR_BIN)
