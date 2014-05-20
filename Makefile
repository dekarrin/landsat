CC=g++
CFLAGS=-Wall

all: test

test: test.o sort.o
	$(CC) -o test test.o sort.o 

%.o: %.cpp %.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o test
