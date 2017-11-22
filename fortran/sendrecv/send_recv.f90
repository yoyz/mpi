   program hello
   implicit none  

   include 'mpif.h'
   integer rank, size, ierr, tag, status(MPI_STATUS_SIZE)
   integer n
   integer TAB(64)
   integer i
   n=64 
   call MPI_INIT(ierr)
   call MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierr)
   call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
   do i=1,n
     TAB(i)=rank*1000+i
   enddo
   print *,rank,TAB(1),TAB(64)
   print*, 'node', rank, ': Hello world'

   if (rank == 0) then
     call MPI_Send(TAB, n, MPI_INT, 1, 0, MPI_COMM_WORLD,ierr)
   end if
   if (rank == 1) then
     call MPI_Recv(TAB, n, MPI_INT, 0, 0, MPI_COMM_WORLD,status,ierr)
   end if
   print *,""
   print *,rank,TAB(1),TAB(64)
   print *, 'node', rank, ': Hello world'

   call MPI_FINALIZE(ierr)
   end
