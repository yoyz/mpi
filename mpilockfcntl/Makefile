# Makefile

# source /opt/intel/bin/iccvars.sh intel64
# source /opt/intel/mpi-rt/4.0.3/bin64/mpivars.sh
SRC_BB	= fcntl_lock_mpi.c
OBJ_BB	= fcntl_lock_mpi.o
BIN_BB	= fcntl_lock_mpi


CC	= icc
MPICC	= mpicc
CFLAGS	= -align -I/opt/intel/impi/4.0.3/include64/ -L/opt/intel/impi/4.0.3/lib64/ -lmpi
LDLIBS	=

all: $(BIN_BB)


BIN: $(BIN_BB)
	$(MPICC) $(CFLAGS) $(OBJ_BB) -o $(BIN_BB)   $(LDLIBS)

OBJ: $(OBJ_BB)
	$(MPICC) $(CFLAGS) $(SRC_BB) -o $(OBJ_BB)

clean:
	rm -f $(BIN_BB) $(OBJ_BB)

