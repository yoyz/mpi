#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main( int argc, char *argv[] )
{
	MPI_File fh;
	MPI_Comm comm;
	int rank,size;
	int64_t i;
	int array_size=atoi(argv[1]);
	int repeat=atoi(argv[2]);
	char * solution=malloc(sizeof(char)*array_size);
	if (argc!=3) { printf("mpiio <xferByte> <repeat>\n"); exit(1); }
	MPI_Init(&argc,&argv);
	comm = MPI_COMM_WORLD;
	MPI_Comm_rank( comm, &rank );
	MPI_Comm_size( comm, &size );

	memset(solution,rank,array_size);
	MPI_File_open(comm,"output",MPI_MODE_WRONLY|MPI_MODE_CREATE,MPI_INFO_NULL,&fh);
	MPI_File_set_view(fh, rank*array_size*sizeof(char), MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
	for (i=1;i<repeat+1;i++)
	{
		MPI_File_write_all( fh, solution, array_size, MPI_CHAR, MPI_STATUS_IGNORE );
		MPI_File_seek( fh, array_size*size*i, MPI_SEEK_SET );
	}
	MPI_File_close( &fh );
	MPI_Finalize();      
} 
