#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>
#include <stdint.h>  // int64_t 
#include <string.h>  // memcpy
#include <stdlib.h>  // malloc/free



int64_t MSGSIZE=1024*1024*64;
//long MSGSIZE=1<<23;
int64_t N      =200;

double stoptime(void) 
{
  struct timeval t;
  gettimeofday(&t,NULL);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}


int main()
{

  int     world_rank;
  int     world_size;
  int     number;
  //char *  tab        = malloc(sizeof(char)*MSGSIZE);
  char   * tab;
  
  char ** tab_hostname;
  char *  myhostname = malloc(sizeof(char)*256);
  double  msgsize_n  = MSGSIZE*N*(sizeof(char));
  double  t;
  int     i;
  int     j;
  int     intsize=sizeof(int);

  posix_memalign(&tab,4096,MSGSIZE);

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  gethostname(myhostname,256);
  printf("%s\n",myhostname);
  tab_hostname = malloc(sizeof(char *)*world_size);  
  for (i=0;i<world_size;i++)
    tab_hostname[i]=malloc(sizeof(char)*256);
  
  if (world_rank==0)
    {
      memcpy(tab_hostname[0],myhostname,256);
      for (i=1;i<world_size;i++)
	MPI_Recv(tab_hostname[i], 256, MPI_CHAR, i, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("Rank:%d Size:%d\n",world_rank,world_size);
    }
  else
    {
      MPI_Send(myhostname, 256, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
  
  if (world_rank < world_size/2)
    {
      int send=(world_size-1)-world_rank;

      for (i=0;i<MSGSIZE;i++)
	tab[i]=1;     
      printf("snd %d > %d\n",world_rank,send);
      t=stoptime();
      for (i=0;i<N;i++)
	MPI_Send(tab, MSGSIZE, MPI_CHAR,send, 0, MPI_COMM_WORLD);
      t=stoptime()-t;
      printf("rank %d transfer %f in %f s : %f Mo/s\n",world_rank,(float)MSGSIZE*N,t,(double)msgsize_n/(t*1000*1000));
    } 
  else if ((world_rank>= world_size/2)==1) 
    {
      int recv=world_size-world_rank-1;
      printf("rcv %d < %d\n",world_rank,recv);
      for (i=0;i<N;i++)
	MPI_Recv(tab, MSGSIZE, MPI_CHAR, recv, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
  MPI_Barrier(MPI_COMM_WORLD);  
  free(tab);
  MPI_Finalize();

}
