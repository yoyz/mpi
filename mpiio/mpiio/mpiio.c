#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main( int argc, char *argv[] )
{
        struct timeval tv0;
        struct timeval tv1;
        struct timezone tz;
        int64_t t;
        double data, time;
        char * filename;
        MPI_File fh;
        MPI_Comm comm;
        int rank,size;
        int64_t i,size64,array_size,repeat;
        char * solution;
        if (argc  < 3 || argc > 4)      { printf("mpiio <xferByte> <repeat> [filename]\n");     exit(1);        }
        if (argc == 4 )                 { filename=(char*)argv[3]; }    else { filename="output";               }
        array_size=atoi(argv[1]);
        repeat=atoi(argv[2]);
        solution=malloc(sizeof(char)*array_size);
        MPI_Init(&argc,&argv);
        comm = MPI_COMM_WORLD;
        MPI_Comm_rank( comm, &rank );
        MPI_Comm_size( comm, &size );
        size64=size;
        memset(solution,rank,array_size);
        t=0;
        gettimeofday(&tv0,&tz);
        MPI_File_open(comm,filename,MPI_MODE_WRONLY|MPI_MODE_CREATE,MPI_INFO_NULL,&fh);
        MPI_File_set_view(fh, rank*array_size*sizeof(char), MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
        for (i=1;i<repeat+1;i++)
        {
                MPI_File_write_all( fh, solution, array_size, MPI_CHAR, MPI_STATUS_IGNORE );
                MPI_File_seek( fh, array_size*size*i, MPI_SEEK_SET );
        }
        MPI_File_close( &fh );
        gettimeofday(&tv1,&tz);
        t+=(tv1.tv_sec-tv0.tv_sec)*1000000 + tv1.tv_usec-tv0.tv_usec;
        if (rank==0)
        {
                data=(double)(array_size*size64*repeat);
                time=t; time=time/1000000.0;
                printf("BW: %.1lf MB/s data: %.1lf MB Time spent: %.1lf s \n",(data/time)/1000/1000,data/1000/1000,time);
        }
        MPI_Finalize();
}
