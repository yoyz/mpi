#include <mpi.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int size, rank;
char hostname[1024];

int check_lock(char * filename)
{
 int fd;
 struct flock lock;
 int retcode=0;

 printf ("%s rank %d open %s\n", hostname,rank,filename);
 fd = open (filename, O_WRONLY);
 memset (&lock, 0, sizeof(lock));
 lock.l_type = F_WRLCK;

 if (rank== 0)  { 
		printf("%s rank %d locking %s\n",hostname,rank,filename);
		retcode=fcntl (fd, F_SETLKW, &lock);
		if (retcode!=0) { printf("%s rank %d error %d\n",hostname,rank,retcode); }
		}
 
 MPI_Barrier(MPI_COMM_WORLD); // here rank 0 has lock

 printf("%s rank %d fcntl lock write filename %s\n",hostname,rank,filename);
 fcntl (fd, F_SETLKW, &lock); // here rank 1..X should be blocked by fcntl

 printf ("%s rank %d gotlock %s\n",hostname,rank,filename);
 if (rank==0) { printf("=== Rank 0 should be the first to gotlock and unlock rank line should be group by two ===\n"); sleep(2); }

 printf ("%s rank %d fcntl unlock write %s\n",hostname,rank,filename);
 lock.l_type = F_UNLCK;
 fcntl (fd, F_SETLKW, &lock); //rank 0 should be the first here to unlock

 close (fd);
 return 0;
}

int main(int argc, char **argv) 
{

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  gethostname(hostname, 1023);
  printf("%s rank %d hello world\n", hostname, rank);
  check_lock(argv[1]);
  MPI_Finalize();
}
