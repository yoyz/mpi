#include "hdf5.h"
#include "stdlib.h"
#include <sys/time.h>

#define H5FILE_NAME     "output.h5"
#define DATASETNAME 	"CharArray" 
#define ALIGNEMENT 1024*1024
int main (int argc, char **argv)
{
    hid_t       file_id, dset_id;         /* file and dataset identifiers */
    hid_t       filespace, memspace;      /* file and memory dataspace identifiers */
    hsize_t     dimsf[2];                 /* dataset dimensions */
    char        *data;                    /* pointer to data buffer to write */
    hsize_t	count[2];	          /* hyperslab selection parameters */
    hsize_t	offset[2];
    hid_t	plist_id;                 /* property list identifier */
    int         i;
    herr_t	status;
    int         repeat_count=0;
    int         nx,ny,repeat;
    int64_t     t=0;
    double      data_written=0;
    double      time=0;
    struct timeval tv0;
    struct timeval tv1;
    struct timezone tz;    
    int mpi_size, mpi_rank;
    MPI_Comm comm  = MPI_COMM_WORLD;
    MPI_Info info  = MPI_INFO_NULL;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &mpi_size);
    MPI_Comm_rank(comm, &mpi_rank);  

    if (argc!=3) { printf("hyperslab_by_row <xfer> <repeat>\n"); exit(1); }
    nx=mpi_size;
    ny=atoi(argv[1]);         // xfer
    repeat=atoi(argv[2]);     // repeat
    data_written=nx*ny*repeat;
      
    gettimeofday(&tv0,&tz);
    plist_id = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(plist_id, comm, info);
    H5Pset_alignment(plist_id,ALIGNEMENT,ALIGNEMENT);

    file_id = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);
    H5Pclose(plist_id);
   
    dimsf[0] = nx;        // number of element write xfer 
    dimsf[1] = ny*repeat; // number of 'slice'
    filespace = H5Screate_simple(2, dimsf, NULL); 

    dset_id = H5Dcreate(file_id, DATASETNAME, H5T_NATIVE_CHAR, filespace,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Sclose(filespace);
    count[0] = (nx/mpi_size);
    count[1] = (ny);
    memspace = H5Screate_simple(2, count, NULL);
    data = (char *) malloc(sizeof(char)*count[0]*count[1]);
    for (i=0; i < count[0]*count[1]; i++) {
        data[i] = mpi_rank + 10;
    }
    
    filespace = H5Dget_space(dset_id);

    plist_id = H5Pcreate(H5P_DATASET_XFER);
    H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);

    for (repeat_count=0;repeat_count<repeat;repeat_count++)
      {
	offset[0] = mpi_rank * count[0];
	offset[1] = repeat_count*ny;	
	H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, NULL);
	status = H5Dwrite(dset_id, H5T_NATIVE_CHAR, memspace, filespace,
			  plist_id, data);
      }
    
    free(data);

    H5Dclose(dset_id);
    H5Sclose(filespace);
    H5Sclose(memspace);
    H5Pclose(plist_id);
    H5Fclose(file_id);
    gettimeofday(&tv1,&tz);
    t+=(tv1.tv_sec-tv0.tv_sec)*1000000 + tv1.tv_usec-tv0.tv_usec;
    if (mpi_rank==0)
      {
	time=t;
	time=time/1000000.0;
	printf("BW: %.1lf MB/s data: %.1lf MB Time spent: %.1lf s \n",(data_written/time)/1000/1000,data_written/1000/1000,time);
      }    
    
    MPI_Finalize();

    return 0;
}     
