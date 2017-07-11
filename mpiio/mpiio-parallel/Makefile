# Makefile

SRC	= mpiio-test.c
OBJ	= mpiio-test.o
BIN	= mpiio-test


CC	= mpicc
MPICC	= mpicc
CFLAGS	=  -lmpi
LDLIBS	=

all: $(BIN)


BIN: $(BIN)
	$(MPICC) $(CFLAGS) $(OBJ) -o $(BIN)   $(LDLIBS)

OBJ: $(OBJ)
	$(MPICC) $(CFLAGS) $(SRC) -o $(OBJ)

clean:
	rm -f $(BIN) $(OBJ)

