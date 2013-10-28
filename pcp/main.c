#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

//#define MSGSIZE 1000000
//#define N       1000

long MSGSIZE=1024*1024;
//long MSGSIZE=1<<23;
//long N      =10;

double stoptime(void) 
{
  struct timeval t;
  gettimeofday(&t,NULL);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}


int main(char * argc,char **argv)
{
  long     N;
  long     O;
  int      world_rank;
  int      world_size;
  int      number;
  char **  hostname;
  char *   myhostname = malloc(sizeof(char)*256);
  int  *   tab        = malloc(sizeof(int)*MSGSIZE);
  double   msgsize_n  = MSGSIZE*N*(sizeof(int));
  double   t;
  int      i;
  int      j;
  int      intsize=sizeof(int);

  int8_t * buff;
  FILE   * RFD;
  FILE   * WFD;

  int      rindex=0;
  int      windex=0;
  int      rcount=0;
  int      wcount=0;

  long    * file_size;
 
  struct stat statstruct;

  file_size=malloc(sizeof(long));
  buff=malloc(sizeof(int8_t)*1024*1024);

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  gethostname(myhostname,256);
  hostname = malloc(sizeof(char *)*world_size);  

  if (world_rank==0)
    {
      stat(argv[1], &statstruct);
      *file_size=statstruct.st_size;
    }

  if (world_rank==0)
    for (i=1;i<world_size;i++)
      MPI_Send(file_size, 1, MPI_LONG,i, 0, MPI_COMM_WORLD);
  
  if (world_rank!=0)
    {
      MPI_Recv(file_size, 1, MPI_LONG,0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("size:%d",*file_size);
    }

  N=(*file_size)/MSGSIZE;
  O=(*file_size)%MSGSIZE;
  //printf("\n\nN:%d O:%d\n\n",N,O);
  printf("Rank:%d Size:%d\n",world_rank,world_size);
  if (world_rank==0)
    {
      RFD=fopen(argv[1],"r+");
      for (i=0;i<N;i++)
	{
	  rcount=fread(buff,MSGSIZE,sizeof(int8_t),RFD);	  
	  MPI_Send(buff, MSGSIZE, MPI_BYTE,1, 0, MPI_COMM_WORLD);
	}
      if (O>0)
	{
	  rcount=fread(buff,O,sizeof(int8_t),RFD);	  
	  MPI_Send(buff, O, MPI_BYTE,1, 0, MPI_COMM_WORLD);
	}
      t=stoptime()-t;

    } 
  else if (world_rank==1) 
    {
      WFD=fopen(argv[2],"w+");
      for (i=0;i<N;i++)
	{
	  MPI_Recv(buff, MSGSIZE, MPI_BYTE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  fwrite(buff,MSGSIZE,sizeof(int8_t),WFD);
	}
      if (O>0)
	{
	  MPI_Recv(buff, O, MPI_BYTE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  fwrite(buff,O,sizeof(int8_t),WFD);
	}
    }
  MPI_Barrier(MPI_COMM_WORLD);  
  MPI_Finalize();

  free(tab);
  free(buff);
  free(file_size);


}



