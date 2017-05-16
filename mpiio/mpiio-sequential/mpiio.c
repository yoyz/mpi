#include <stdio.h>
#include <mpi.h>
#define MAX 1000
int main(int argc, char *argv[])
{
 MPI_File fh;
 int buf[MAX], rank;
 MPI_Init(0,0);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 MPI_File_open(MPI_COMM_WORLD, "test.out",
 MPI_MODE_CREATE|MPI_MODE_WRONLY,
 MPI_INFO_NULL, &fh);
 if (rank == 0)
   MPI_File_write(fh, buf, MAX, MPI_INT, MPI_STATUS_IGNORE);
 MPI_File_close(&fh);
 MPI_Finalize();
 return 0;
} 
