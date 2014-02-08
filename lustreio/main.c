#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

//#define MSGSIZE 1000000
//#define N       1000

long MSGSIZE=1024*1024*1;
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

  struct flock lock;
  struct stat statstruct;

  memset (&lock, 0, sizeof(lock));
  lock.l_type = F_WRLCK;

  file_size=malloc(sizeof(long));
  buff=malloc(sizeof(int8_t)*MSGSIZE);

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
  MPI_Barrier(MPI_COMM_WORLD);  

  N=(*file_size)/MSGSIZE;
  O=(*file_size)%MSGSIZE;
  //printf("\n\nN:%d O:%d\n\n",N,O);
  printf("Rank:%d Size:%d\n",world_rank,world_size);
  if (world_rank==0)
    {
      RFD=fopen(argv[1],"r+");
      MPI_Barrier(MPI_COMM_WORLD);  
      for (i=0;i<N;i++)
	{
	  //	  printf("=> %d snd_to:%d segment:%d last rcount:%d\n",world_rank,(i)%(world_size-1)+1,i,rcount);
	  rcount=fread(buff,MSGSIZE,sizeof(int8_t),RFD);	  
	  MPI_Send(buff, MSGSIZE, MPI_BYTE,((i)%(world_size-1))+1, 0, MPI_COMM_WORLD);
	  //	  printf("=> %d snd OK\n",0);
	  //	  MPI_Barrier(MPI_COMM_WORLD);  
	}
      printf("[first pass file send OK]\n");
    
      if (O>0)
	{
	  //	  printf("%d snd\n",world_rank);
	  rcount=fread(buff,O,sizeof(int8_t),RFD);	  
	  MPI_Send(buff, O, MPI_BYTE,1, 0, MPI_COMM_WORLD);
	}
    
      t=stoptime()-t;
      fclose(RFD);
    } 
  else if (world_rank!=0) 
    {
      WFD=fopen(argv[2],"w+");
      if (world_rank==1)
	posix_fallocate((int)WFD,0,*file_size);
      MPI_Barrier(MPI_COMM_WORLD);  
      for (i=0;i<N;i++)
	{
	  if (world_rank==((i)%(world_size-1))+1  )
	    {
	      //printf("<= %d recv from %d last wcount:%d\n",world_rank,0,wcount);
	      MPI_Recv(buff, MSGSIZE, MPI_BYTE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	      //  MPI_Barrier(MPI_COMM_WORLD);  
	      //	      lock.l_type = F_WRLCK;
	      //	      fcntl ((int)WFD, F_SETLKW, &lock); // here rank 1..X should be blocked by fcntl
	      //lseek((int)WFD,MSGSIZE*i,SEEK_SET);
	      fseek((int)WFD,MSGSIZE*i,SEEK_SET);
	      wcount=fwrite(buff,MSGSIZE,sizeof(int8_t),WFD);
	      //wcount=fwrite(buff,1,sizeof(int8_t)*1024*1024,WFD);
	      //wcount=write(buff,MSGSIZE,sizeof(int8_t),WFD);
	      //wcount=write(WFD,buff,MSGSIZE);
	      //	      lock.l_type = F_UNLCK;
	      //fcntl ((int)WFD, F_SETLKW, &lock); //rank 0 should be the first here to unlock

	      //	      printf("=> %d rcv OK\n",world_rank);
	    }
	}
      printf("[first pass file receive OK]\n");
    
      fflush(WFD);
      if (O>0 && world_rank==1)
	{
	  printf("%d recv\n",world_rank);
	  MPI_Recv(buff, O, MPI_BYTE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  fseek((int)WFD,MSGSIZE*N,SEEK_SET);
	  fwrite(buff,O,sizeof(int8_t),WFD);
	}
      fflush(WFD);
      fclose(WFD);
    }
  //  
  //MPI_Barrier(MPI_COMM_WORLD);  
  //  fclose(WFD);
  //  fclose(RFD);

  free(tab);
  free(buff);
  free(file_size);


  MPI_Barrier(MPI_COMM_WORLD);  
  MPI_Finalize();



}



