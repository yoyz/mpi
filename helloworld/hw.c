#include <stdio.h>
#include <mpi.h>

main(int argc, char **argv) 
{
  int size, rank;
  char hostname[1024];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  hostname[1023] = '\0';
  gethostname(hostname, 1023);
  printf("Hello world, rank : %.4d, size : %.4d, hostname : %s\n", rank, size, hostname);
  MPI_Finalize();
}
