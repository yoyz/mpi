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
    double t1_send,     t2_send;
    double t1_recv,     t2_recv;
    double t1_sendrecv, t2_sendrecv;
    size_t alignement=16;
    MPI_Status status;
    MPI_Request request, request2;

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

    posix_memalign((void**)&sb,16,size*chunk*sizeof(uint8_t));
    if ( !sb ) {
        perror( "can't allocate send buffer" );fflush(stderr);
        MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }

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
    // Sending data from rank [ 0,...,n/2] to [ n/2+1, n-1 ]
    t1_send = MPI_Wtime();
    for (i=0;i<iter;i++)
      {
	if (rank <  size/2) { MPI_Send(sb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD);            }
	if (rank >= size/2) { MPI_Recv(rb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD, &status);   }	
      }
    t2_send = MPI_Wtime();

    // Sending data from rank [ n/2+1, n-1 ] to [ 0,...,n/2]
    t1_recv = MPI_Wtime();
    for (i=0;i<iter;i++)
      {
	if (rank <  size/2) { MPI_Recv(rb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD, &status);  }
	if (rank >= size/2) { MPI_Send(sb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD);           }	
      }
    t2_recv = MPI_Wtime();

    
    // Sending and receiving data  from rank [ n/2+1, n-1 ] to [ 0,...,n/2]
    t1_sendrecv = MPI_Wtime();
    for (i=0;i<iter;i++)
      {
	if (rank <  size/2)
	  {
	    MPI_Isend(sb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD, &request);
	    MPI_Irecv(rb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 456, MPI_COMM_WORLD, &request2);
	  }
	if (rank >= size/2)
	  {
	    MPI_Isend(sb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 456, MPI_COMM_WORLD, &request);
	    MPI_Irecv(rb, chunk, MPI_UNSIGNED_CHAR, size-(rank+1), 123, MPI_COMM_WORLD, &request2);
	  }
	MPI_Wait(&request,  &status);
	MPI_Wait(&request2, &status);
      }
    t2_sendrecv = MPI_Wtime();
    

    
    if (rank==0) printf("iterTime:%.2f dataMB:%d BWSendMB/s:%.4f BWRecvMB/s:%.4f BWSendRecvMB/s:%.4f iter:%d\n",
			((t2_send-t1_send)+(t2_recv-t1_recv)+(t2_sendrecv-t1_recv))/(iter*3),
			(chunk)/1000/1000,
			(chunk*iter)/(t2_send-t1_send)        /1000/1000,
			(chunk*iter)/(t2_recv-t1_recv)        /1000/1000,
			(chunk*iter)/(t2_sendrecv-t1_sendrecv)/1000/1000,
			iter);
    MPI_Barrier(MPI_COMM_WORLD);
    
    free(sb);
    free(rb);
    MPI_Finalize();
    return(EXIT_SUCCESS);
}
