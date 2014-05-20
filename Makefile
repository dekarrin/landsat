CC=g++
CFLAGS=-Wall
DEPS=sort.o util.o
BINDIR=bin

all: test

test: test.o $(DEPS)
	mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/test test.o $(DEPS)

%.o: %.cpp %.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf $(BINDIR)
