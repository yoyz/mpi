/*
 * %Id: mkrandpfile.c,v 1.13 2003/10/19 19:29:59 gustav Exp %
 *
 * %Log: mkrandpfile.c,v %
 * Revision 1.13  2003/10/19 19:29:59  gustav
 * Indented the file with Emacs.
 *
 * Revision 1.12  2003/10/19 19:26:09  gustav
 * Truncated the log.
 *
 *
 */

#include <stdio.h>   /* all IO stuff lives here */
#include <stdlib.h>  /* exit lives here */
#include <unistd.h>  /* getopt lives here */
#include <string.h>  /* strcpy lives here */
#include <mpi.h>     /* MPI and MPI-IO live here */
#include <stdint.h>
#define MASTER_RANK 0
#define TRUE 1
#define FALSE 0
#define BOOLEAN int
//#define BLOCK_SIZE 1048576
#define MBYTE 1048576
#define SYNOPSIS printf ("synopsis: %s -f <file> -n <numberOfBlock> -b <blocksInByte> -r <repeat>\n", argv[0])

int main(int argc, char ** argv)
{
  /* my variables */

  int          my_rank, pool_size, count;
  BOOLEAN      i_am_the_master = FALSE, input_error = FALSE;
  char        *filename = NULL;

  char        *junk;

  int64_t          number_of_blocks=0;
  int64_t          block_size_in_byte=0;
  int64_t          number_of_repeat=0;
  int64_t          filename_length=0;
  int64_t          total_number_of_bytes=0;
  int64_t          number_of_bytes_by_repeat=0;

  //printf("%d\n",sizeof(int)); exit(0);

  MPI_Offset   my_offset, my_current_offset;
  MPI_File     fh;
  MPI_Status   status;
  double       start, finish, io_time, longest_io_time;

  /* getopt variables */

  extern char *optarg;
  int c;
  int i,j,k;

  /* ACTION */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &pool_size);



  MPI_Info mpi_info;
  mpi_info = MPI_INFO_NULL;

  MPI_Info_create(&mpi_info);
  MPI_Info_set(   mpi_info, "direct_io","true");


  /*
  MPI_Info_create(&mpi_info);
  //MPI_Info_set(   mpi_info, "cb_block_size", "1024" );
  //MPI_Info_set(   mpi_info, "cb_buffer_size","1024");
  //MPI_Info_set(   mpi_info, "cb_block_size", "1048576" );
  //MPI_Info_set(   mpi_info, "cb_buffer_size","1048576");

  MPI_Info_set(   mpi_info, "cb_block_size", "16777216" );
  MPI_Info_set(   mpi_info, "cb_buffer_size","16777216");

  MPI_Info_set(   mpi_info, "cb_nodes","60");

  MPI_Info_set(   mpi_info, "romio_cb_write","true");
  MPI_Info_set(   mpi_info, "romio_cb_read","true");

  //MPI_Info_set(   mpi_info, "cb_buffer_size","16777216");
  //MPI_Info_set(   mpi_info, "cb_config_list","sid602");
  MPI_Info_set(   mpi_info, "collective_buffering", "true" );
  MPI_Info_set(   mpi_info, "striping_unit","16777216");
  MPI_Info_set(   mpi_info, "striping_factor","16");
  MPI_Info_set(   mpi_info, "direct_io","false");
  */


  if (my_rank == MASTER_RANK) i_am_the_master = TRUE;

  if (i_am_the_master)
    {

      /* read the command line */
      while ((c = getopt(argc, argv, "f:n:b:r:h")) != EOF)
        switch(c)
          {
          case 'f':
            filename = optarg;
#ifdef DEBUG
            printf("output file: %s\n", filename);
#endif
            break;
          case 'n':
            if ((sscanf (optarg, "%d", &number_of_blocks) != 1) ||
                (number_of_blocks < 1))
              {
                SYNOPSIS;
                input_error = TRUE;
              }
            break;
          case 'b':
            if ((sscanf (optarg, "%d", &block_size_in_byte) != 1) ||
                (block_size_in_byte < 1))
              {
                SYNOPSIS;
                input_error = TRUE;
              }
            break;
          case 'r':
            if ((sscanf (optarg, "%d", &number_of_repeat) != 1) ||
                (number_of_repeat < 1))
              {
                SYNOPSIS;
                input_error = TRUE;
              }
          }

            printf("Each process will write %I64d blocks of byte and repeat %I64d\ntotal:%I64d\n",
                   number_of_blocks*block_size_in_byte,number_of_repeat,number_of_blocks*block_size_in_byte*number_of_repeat);

  /* Check if the command line has initialized filename and
   * number_of_blocks.
   */

  if ((filename == NULL) || (number_of_blocks == 0)) {
    SYNOPSIS;
    input_error = TRUE;
  }

  if (input_error) MPI_Abort(MPI_COMM_WORLD, 1);
  /* This is another way of exiting, but it can be done only
     if no files have been opened yet. */

    filename_length = strlen(filename) + 1;

} /* end of "if (i_am_the_master)"; reading the command line */

    /* If we got this far, the data read from the command line
       should be OK. */

  MPI_Bcast(&number_of_blocks,   1, MPI_LONG_LONG, MASTER_RANK, MPI_COMM_WORLD);
  MPI_Bcast(&block_size_in_byte, 1, MPI_LONG_LONG, MASTER_RANK, MPI_COMM_WORLD);
  MPI_Bcast(&number_of_repeat,   1, MPI_LONG_LONG, MASTER_RANK, MPI_COMM_WORLD);
  MPI_Bcast(&number_of_repeat,   1, MPI_LONG_LONG, MASTER_RANK, MPI_COMM_WORLD);
  MPI_Bcast(&filename_length,    1, MPI_LONG_LONG, MASTER_RANK, MPI_COMM_WORLD);
  if (! i_am_the_master) filename = (char*) malloc(filename_length);
  MPI_Bcast(filename, filename_length, MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
#ifdef DEBUG
  printf("%3d: number_of_repeat: %d\n",             my_rank,number_of_repeat);
  printf("%3d: number_of_blocks: %d\n",             my_rank,number_of_blocks);
  printf("%3d: received block_size_in_byte : %d\n", my_rank,block_size_in_byte);
  printf("%3d: received broadcast\n",               my_rank);
  printf("%3d: filename = %s\n",                    my_rank, filename);
#endif

  MPI_Barrier(MPI_COMM_WORLD);
  /* number_of_bytes must be just plain integer, because we are
     going to use it in malloc */

  total_number_of_bytes =
    pool_size * block_size_in_byte*number_of_blocks * number_of_repeat;
  number_of_bytes_by_repeat =
    pool_size * block_size_in_byte*number_of_blocks;


#ifdef DEBUG
if (i_am_the_master)
  {
    printf("block_size_in_byte         = %d\n",         block_size_in_byte);
    printf("number_of_bytes_by_repeat  = %d\n", number_of_bytes_by_repeat);
    printf("total_number_of_bytes      = %d\n",       total_number_of_bytes);
    //printf("size of offset        = %d bytes\n",   sizeof(MPI_Offset));
  }
#endif
/*
 MPI_File_open(MPI_COMM_WORLD, filename,
               MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
*/
 MPI_File_open(MPI_COMM_WORLD, filename,
               MPI_MODE_CREATE | MPI_MODE_WRONLY, mpi_info, &fh);

 i=0;
 j=1;
 k=0;

 printf("allocate:%d %d\n",block_size_in_byte*sizeof(char));

 junk = (char*) malloc(block_size_in_byte*sizeof(char));
 //junk = (char*) malloc(1024*1024);
 printf("junk:%d\n",junk);
 //junk = (char*) malloc(1024*1024*sizeof(int));
 //printf("0%ld\n",junk);
 /*
 for (i = 0; i < block_size_in_byte; i++)
   {
     *(junk + i) = my_rank;
     printf(".");
   }
 */
 memset(junk,my_rank,block_size_in_byte*sizeof(char));
 printf("filled\n");

 start = MPI_Wtime();
 while (j<number_of_repeat+1)
   {
     //MPI_Barrier(MPI_COMM_WORLD);
     //my_offset = block_size_in_byte*my_rank*number_of_blocks*j;
     my_offset = block_size_in_byte*number_of_blocks*my_rank;

     my_offset= my_offset+number_of_bytes_by_repeat*(j-1);

     MPI_File_seek(fh, my_offset, MPI_SEEK_SET);
     MPI_File_get_position(fh, &my_current_offset);
#ifdef DEBUG
     //MPI_Barrier(MPI_COMM_WORLD);
     printf ("%3d: my current offset is %lld %lld\n", my_rank, my_offset,my_current_offset);
#endif

     /* generate random integers */


     //srand(28 + my_rank);
     //for (i = 0; i < number_of_integers; i++) *(junk + i) = rand();
     /* write the stuff out */

     k=0;

     //{
     //for (k=0;k<number_of_blocks;k++)

     while (k<number_of_blocks)
       {
#ifdef DEBUG2
         printf("%3d: wrote %d %d/%d : %d\n", my_rank,block_size_in_byte,k,number_of_blocks,k<number_of_blocks);
#endif
         MPI_File_write(fh, junk, block_size_in_byte, MPI_CHAR, &status);
         k++;
         //}
       }


#ifdef DEBUG
     MPI_Get_count(&status, MPI_INT, &count);
     printf("%3d: wrote %d integers number_of_repeat:%I64d/%I64d\n", my_rank, count,j,number_of_repeat);
#endif
     
#ifdef DEBUG
     MPI_File_get_position(fh, &my_current_offset);
     printf ("%3d: my current offset is %lld\n", my_rank, my_current_offset);
#endif
     j++;
   }
 MPI_File_close(&fh);

 finish = MPI_Wtime();
 io_time = finish - start;



 MPI_Allreduce(&io_time, &longest_io_time, 1, MPI_DOUBLE, MPI_MAX,
               MPI_COMM_WORLD);

 if (i_am_the_master)
   {
     printf("longest_io_time       = %f seconds\n", longest_io_time);
     printf("total_number_of_bytes = %lld\n", total_number_of_bytes);
     printf("transfer rate         = %f MB/s\n",
            total_number_of_bytes / longest_io_time / MBYTE);
   }

 MPI_Finalize();
 exit(0);
}

