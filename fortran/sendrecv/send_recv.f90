   program hello
   use mpi
   implicit none  

   integer rank, size, ierr, tag, status(MPI_STATUS_SIZE)
   integer n
   integer, parameter :: TABSIZE = 1024*1024*32
   integer TAB(TABSIZE)
   integer i
   double precision start,stop 
   n=TABSIZE
   call MPI_INIT(ierr)
   call MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierr)
   call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
   do i=1,n
     TAB(i)=rank*1000+i
   enddo
   print *,rank,TAB(1),TAB(TABSIZE)
   !print*, 'node', rank, ': Hello world'
   start=MPI_WTIME()
   if (rank == 0) then
     call MPI_Send(TAB, n, MPI_INT, 1, 0, MPI_COMM_WORLD,ierr)
   end if
   if (rank == 1) then
     call MPI_Recv(TAB, n, MPI_INT, 0, 0, MPI_COMM_WORLD,status,ierr)
   end if
   stop=MPI_WTIME()
   write(rank+1000,"(i0,f7.3)") rank,stop-start

   call   MPI_Barrier(MPI_COMM_WORLD,ierr)
   print *,"===="
   call   MPI_Barrier(MPI_COMM_WORLD,ierr)
   print *,rank,TAB(1),TAB(TABSIZE),stop-start

   call MPI_FINALIZE(ierr)
   end
