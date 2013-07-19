# Makefile

SRC	= hw.c
OBJ	= hw.o
BIN	= hw


CC	= mpicc
MPICC	= mpicc
CFLAGS	=  -lmpi -I/usr/lib/openmpi/include/
LDLIBS	=

all: $(BIN)


BIN: $(BIN)
	$(MPICC) $(CFLAGS) $(OBJ) -o $(BIN)   $(LDLIBS)

OBJ: $(OBJ)
	$(MPICC) $(CFLAGS) $(SRC) -o $(OBJ)

clean:
	rm -f $(BIN) $(OBJ)

