#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

int main( int argc, char *argv[] )
{
    int rank, size;
    uint64_t chunk = 128;
    uint64_t iter  = 1;
    int i;
    uint8_t *sb;
    uint8_t *rb;
    //int status, gstatus;
    double t1, t2;
    size_t alignement=16;
    MPI_Status status;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    for ( i=1 ; i < argc ; ++i )
      {
        if ( argv[i][0] != '-' )
	  continue;
        switch(argv[i][1])
	  {
	  case 'm':
	    chunk = atoi(argv[++i]);
	    break;

	  case 'i':
	    iter = atoi(argv[++i]);
	    break;	      

	  default:
	    fprintf(stderr, "Unrecognized argument %s\n", argv[i]);fflush(stderr);
	    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);	 
	  }
      }
    //sb = (uint8_t *)memalign(alignement,size*chunk*sizeof(uint8_t));
    posix_memalign((void**)&sb,16,size*chunk*sizeof(uint8_t));
    if ( !sb ) {
        perror( "can't allocate send buffer" );fflush(stderr);
        MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }
    //rb = (uint8_t *)memalign(alignement,size*chunk*sizeof(uint8_t));
    posix_memalign((void**)&rb,16,size*chunk*sizeof(uint8_t));
    if ( !rb ) {
        perror( "can't allocate recv buffer");fflush(stderr);
        free(sb);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    for ( i=0 ; i < size*chunk ; ++i ) {
        sb[i] = rank + 1;
        rb[i] = 0;
    }
    t1 = MPI_Wtime();
    for (i=0;i<iter;i++)
      {
	//MPI_Allgather(sb, chunk, MPI_UNSIGNED_CHAR, rb, chunk, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
	//status = MPI_Alltoall(sb, chunk, MPI_UNSIGNED_CHAR, rb, chunk, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
	//MPI_Allreduce( &status, &gstatus, 1, MPI_UNSIGNED_CHAR, MPI_SUM, MPI_COMM_WORLD );
	if (rank < size/2)
	  {
	    MPI_Send(sb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD);
	  }

	if (rank >= size/2)
	  {
	    //MPI_Send(sb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD);
	    //MPI_Recv(rb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD);
	    MPI_Recv(sb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD, &status);
	  }
	
      }
    t2 = MPI_Wtime();
    /*
    if (rank==0) printf("iterTime:%.2f dataMB:%d BidirBWMB/s:%.4f iter:%d\n",
			(t2-t1)/iter,
			(size*chunk)/1000/1000,
			((size-1)*chunk*iter*2)/(t2-t1)/1000/1000,
			iter);
    */
    if (rank==0) printf("iterTime:%.2f dataMB:%d BidirBWMB/s:%.4f iter:%d\n",
			(t2-t1)/iter,
			(chunk)/1000/1000,
			(chunk*iter)/(t2-t1)/1000/1000,
			iter);
    
    free(sb);
    free(rb);
    MPI_Finalize();
    return(EXIT_SUCCESS);
}
