#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>
#include <stdlib.h>


#ifdef __DEBUG__
#define DEBUG 1
#else
#define DEBUG 0
#endif


long MSGSIZEINBYTE = 1024*1024*64;
long N             = 100;

double  t1,t2,t3;

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
  int  *  tab        = malloc(sizeof(double)*MSGSIZEINBYTE*8 );
  char ** hostname_list;
  double * sendperf_list;
  char *  myhostname = malloc(sizeof(char)*256);
  double  msgsize_n  = MSGSIZEINBYTE*N*(sizeof(int));
  int     i;
  int     j;
  int     intsize=sizeof(int);
  int     doublesize=sizeof(double);

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  gethostname(myhostname,256);
  hostname_list = malloc(sizeof(char *)*world_size);


  //sending hostname to rank0
  if (world_rank==0)
    for (i=1;i<world_size;i++)
      {
        char ** hostname_list2=hostname_list+i;
        hostname_list2=malloc(sizeof(char)*256);
        MPI_Recv(hostname_list2, 256, MPI_CHAR, i, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      }
  else
    {
      if (DEBUG) printf("%s\n",myhostname);
      MPI_Send(myhostname, 256, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

  if (DEBUG) printf("Rank:%d Size:%d\n",world_rank,world_size);

  MPI_Barrier(MPI_COMM_WORLD);

  //begin sending data from
  // rank 0 to rank n-1 , rank 1 to rank n-2 , rank 2 to rank n-3
  if (world_rank < world_size/2)
    {
      int send=(world_size-1)-world_rank;
      int recv=world_size-world_rank-1;
      for (i=0;i<MSGSIZEINBYTE;i++)
        tab[i]=1;
      if (DEBUG) printf("snd %d > %d\n",world_rank,send);
      t1=stoptime();
       for (i=0;i<N;i++)
        MPI_Send(tab, MSGSIZEINBYTE/8, MPI_DOUBLE,send, 0, MPI_COMM_WORLD);
      t1=stoptime()-t1;
      t2=stoptime();
      for (i=0;i<N;i++)
        MPI_Recv(tab, MSGSIZEINBYTE/8, MPI_DOUBLE, recv, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      t2=stoptime()-t2;

      t3=stoptime();
      for (i=0;i<N;i++)
        {
        MPI_Send(tab, MSGSIZEINBYTE/8, MPI_DOUBLE,send, 0, MPI_COMM_WORLD);
        MPI_Recv(tab, MSGSIZEINBYTE/8, MPI_DOUBLE, recv, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
      t3=stoptime()-t3;

      //printf("rank %d %d transfer %f in %f s : %f Mo/s\n",world_rank,send,(double)MSGSIZEINBYTE/8*N*doublesize,t1,(double)msgsize_n/(t1*1024*1024));
      printf("rank %5d %5d MBytes: %.0f  send: %.1f MiB/s receive: %.1f MiB/s, sndreceive: %.1f MiB/s  \n",world_rank,send,
             (double)((MSGSIZEINBYTE/8*N*doublesize)/1024)/1024,
             (double)msgsize_n/(t1*1024*1024),
             (double)msgsize_n/(t2*1024*1024),
             (double)msgsize_n/(t3*1024*1024)
             );

    }

  else if ((world_rank>= world_size/2)==1)
    {
      int send=(world_size-1)-world_rank;
      int recv=world_size-world_rank-1;
      if (DEBUG) printf("rcv %d < %d\n",world_rank,recv);
       for (i=0;i<N;i++)
        MPI_Recv(tab, MSGSIZEINBYTE/8, MPI_DOUBLE, recv, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
       for (i=0;i<N;i++)
        MPI_Send(tab, MSGSIZEINBYTE/8, MPI_DOUBLE,send, 0, MPI_COMM_WORLD);
       for (i=0;i<N;i++)
         {
           MPI_Recv(tab, MSGSIZEINBYTE/8, MPI_DOUBLE, recv, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           MPI_Send(tab, MSGSIZEINBYTE/8, MPI_DOUBLE,send, 0, MPI_COMM_WORLD);
         }
    }
  MPI_Barrier(MPI_COMM_WORLD);


  MPI_Finalize();
  free(tab);

}




