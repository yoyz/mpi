# Makefile

# source /opt/intel/bin/iccvars.sh intel64
# source /opt/intel/mpi-rt/4.0.3/bin64/mpivars.sh
SRC	= gflops.c
OBJ	= gflops.o
BIN	= gflops


CC	= gcc
MPICC	= mpicc
CFLAGS	= -mavx256-split-unaligned-store -I/usr/lib/openmpi/include/ -lmpi
LDLIBS	=

all: $(BIN)


BIN: $(BIN)
	$(MPICC) $(CFLAGS) $(OBJ) -o $(BIN)   $(LDLIBS)

OBJ: $(OBJ)
	$(MPICC) $(CFLAGS) $(SRC) -o $(OBJ)

clean:
	rm -f $(BIN) $(OBJ)
