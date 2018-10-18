CC=mpicc
CXX=mpic++
CFLAGS=-g -O3
LDFLAGS=-lm 

SRC_FILES=mpi_monitor.c

LIB_FILENAME=libmpi_monitor.so
HEADER_FILENAME=mpi_monitor.h
TEST_FILENAME=test.c

PREFIX=/usr/local

default: build

build: so

so: $(SRC_FILES) $(HEADER_FILENAME)
	$(CC) -shared $(CFLAGS) -o $(LIB_FILENAME) -fPIC $(SRC_FILES) $(LDFLAGS)

install: so $(HEADER_FILENAME)
	sudo cp $(LIB_FILENAME) $(PREFIX)/lib64
	sudo cp $(HEADER_FILENAME) $(PREFIX)/include

clean:
	rm $(LIB_FILENAME)

test: so $(TEST_FILENAME)
	$(CC) $(TEST_FILENAME) -lmpi_monitor -o xtest
