#include <stdio.h>
#include <mpi.h>
#define MAX 1024
int main(int argc, char *argv[])
{
 MPI_File fh;
 int buf[MAX], rank, size;
 int position;
 MPI_Init(0,0);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 MPI_Comm_size(MPI_COMM_WORLD, &size);
 MPI_File_open(MPI_COMM_WORLD, "test.out", MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &fh); 
 for (int i=0;i<MAX;i++) buf[i]=rank;
 position=rank*MAX+4096;

 // int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, ROMIO_CONST void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
 // fh 		file handle (handle)
 // offset 	file offset (nonnegative integer)
 // buf 	initial address of buffer (choice)
 // count 	number of elements in buffer (nonnegative integer)
 // datatype 	datatype of each buffer element (handle)
 MPI_File_write_at_all (fh, position, buf, MAX, MPI_CHAR, MPI_STATUS_IGNORE);

 MPI_File_close(&fh);
 MPI_Finalize();
 return 0;
} 
