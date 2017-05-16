#include <stdio.h>
#include <mpi.h>
#define MAX 1000
int main(int argc, char *argv[])
{
 MPI_File fh;
 int buf[MAX], rank, size;
 MPI_Init(0,0);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 MPI_Comm_size(MPI_COMM_WORLD, &size);
 MPI_File_open(MPI_COMM_WORLD, "test.out",
 MPI_MODE_CREATE|MPI_MODE_WRONLY,
 MPI_INFO_NULL, &fh);
 for (int i=0;i<MAX;i++) buf[i]=rank;
 if (rank == 0)
   MPI_File_write(fh, buf, MAX, MPI_INT, MPI_STATUS_IGNORE);
 if (rank==size-1)
 {
   MPI_File_seek(fh,sizeof(MPI_INT)*MAX,MPI_SEEK_SET ); 
   MPI_File_write(fh, buf, MAX, MPI_INT, MPI_STATUS_IGNORE);
 }
 MPI_File_close(&fh);
 MPI_Finalize();
 return 0;
} 
