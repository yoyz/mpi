#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

//#define NBOCTET 1024*1024*1024
//#define NBELEM  NBOCTET/8

#define MEGA    1024*1024


int64_t pattern1=0x0F0F0F0F;
int64_t pattern2=0xF0F0F0F0;


double stoptime(void) 
{
  struct timeval t;
  gettimeofday(&t,NULL);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}


int main(int argc, char **argv)
{
  char      hostname[1024];
  int64_t * buffer; 
  int64_t   i;
  int64_t   j;
  int       rank;
  int       size;
  double    t,t1,t2;
  double    local_bandwith;
  double    global_bandwith;
  int       size_mega_commited_by_rank;
  int       nb_iter;
  
  
  if (argc!=3)
    {
      printf("%s <number_of_mega> <number_of_iter> \n",argv[0]);
      exit(1);
    }

  size_mega_commited_by_rank=atoi(argv[1]);
  nb_iter=atoi(argv[2]);


  //buffer=malloc(sizeof(int64_t)*NBELEM);
  buffer=malloc(sizeof(int64_t)*size_mega_commited_by_rank*MEGA/8);


  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  hostname[1023] = '\0';
  gethostname(hostname, 1023);


  for (j=0;j<nb_iter;j++)
    {
      t1=stoptime();
      for (i=0;i<size_mega_commited_by_rank*MEGA/8;i++)
	{
	  buffer[i]=pattern1;
	}
      for (i=0;i<size_mega_commited_by_rank*MEGA/8;i++)
	{
	  buffer[i]=pattern2;
	}
      t2=stoptime();
      t=t2-t1;
      
      local_bandwith=2*2*size_mega_commited_by_rank/t;       
      MPI_Reduce(&local_bandwith, &global_bandwith, 1, MPI_DOUBLE, MPI_SUM, 0,
		 MPI_COMM_WORLD);

      
      printf("rank:%.4d, size:%.4d, loop:%d time:%f bandwith:%f hostname:%s val:%x\n", rank, size, j, t, local_bandwith, hostname,buffer[1024]);
      MPI_Barrier(MPI_COMM_WORLD);
      if (rank==0)
	printf("                                    global_bandwith:%f\n",global_bandwith);

    }

  MPI_Finalize();
  
}
