/* C source code is found in dgemm_example.c */
// export OMP_NUM_THREADS=24
// export KMP_AFFINITY=compact,granularity=fine,verbose,1
// mpicc -cc=icc   dgemm_3.c  -qopt-streaming-stores always -lmkl_intel_lp64  -lmkl_core -lpthread -fopenmp -lm -mkl=parallel

#define min(x,y) (((x) < (y)) ? (x) : (y))

#include <stdio.h>
#include <stdlib.h>
#include "mkl.h"
#include <mpi.h>

//#define SIZE 4000
//#define SIZE 16000
//#define SIZE 20000
#define SIZE 24000
#ifndef DEBUGPRINTF
#define DEBUGPRINTF 0
#endif

#define NTIME 100

#define DPRINTF(FMT, ARGS...) do { \
  if (DEBUGPRINTF) \
    fprintf(stderr, "%.40s:%.8d [" FMT "]\n", __FUNCTION__, __LINE__, ## ARGS); \
  } while (0)




double stoptime(void) {
  struct timeval t;
  gettimeofday(&t,0);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}


int bench_stream_triad()
{
    double *A, *B, *C;
    double t;
    int64_t m, n, k, i, j;
    m = SIZE, k = SIZE, n = SIZE;
    double scalar=3.14;
    A = (double *)mkl_malloc( m*k*sizeof( double ), 64 );
    B = (double *)mkl_malloc( k*n*sizeof( double ), 64 );
    C = (double *)mkl_malloc( m*n*sizeof( double ), 64 );

#pragma omp parallel for 
    for (i = 0; i < (m*k); i++) {
        A[i] = (double)(i+1);
    }
#pragma omp parallel for 
    for (i = 0; i < (k*n); i++) {
        B[i] = (double)(-i-1);
    }

#pragma omp parallel for 
    for (i = 0; i < (m*n); i++) {
        C[i] = 0.0;
    }

    if (A == NULL || B == NULL || C == NULL) {
      printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
      mkl_free(A);
      mkl_free(B);
      mkl_free(C);
      return 1;
    }
    t=stoptime();
    for (i=0;i<NTIME;i++)
#pragma omp parallel for    
      for (j=0; j<(m*k); j++)
	A[j] = B[j]+scalar*C[j];
    t=stoptime()-t;
    printf("GB/s         : %f\n",(((((m*k)*3)*8)*NTIME)/t)*1E-9);
    DPRINTF("\n Deallocating memory \n\n");
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);
    return 0;
}

int bench_dgemm()
{
    double *A, *B, *C;
    int m, n, k, i, j;
    double alpha, beta;
    double t;

    m = SIZE, k = SIZE, n = SIZE;
    DPRINTF(" Initializing data for matrix multiplication C=A*B for matrix \n"
            " A(%ix%i) and matrix B(%ix%i)\n\n", m, k, k, n);
    alpha = 1.0; beta = 0.0;

    DPRINTF(" Allocating memory for matrices aligned on 64-byte boundary for better \n"
            " performance \n\n");
    A = (double *)mkl_malloc( m*k*sizeof( double ), 64 );
    B = (double *)mkl_malloc( k*n*sizeof( double ), 64 );
    C = (double *)mkl_malloc( m*n*sizeof( double ), 64 );
    if (A == NULL || B == NULL || C == NULL) {
      printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
      mkl_free(A);
      mkl_free(B);
      mkl_free(C);
      return 1;
    }

    DPRINTF(" Intializing matrix data \n\n");
#pragma omp parallel for 
    for (i = 0; i < (m*k); i++) {
        A[i] = (double)(i+1);
    }
#pragma omp parallel for 
    for (i = 0; i < (k*n); i++) {
        B[i] = (double)(-i-1);
    }

#pragma omp parallel for 
    for (i = 0; i < (m*n); i++) {
        C[i] = 0.0;
    }

    DPRINTF(" Computing matrix product using Intel(R) MKL dgemm function via CBLAS interface \n\n");
    t=stoptime();
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                m, n, k, alpha, A, k, B, n, beta, C, n);
    t=stoptime()-t;
    printf("calculation time : %f\n",t);
    printf("gflops/s         : %f\n",((2.0*m*n*k)*1E-9)/t);

    DPRINTF("\n Computations completed.\n\n");

    DPRINTF(" Top left corner of matrix A: \n");
    for (i=0; i<min(m,6); i++) {
      for (j=0; j<min(k,6); j++) {
        DPRINTF("%12.0f", A[j+i*k]);
      }
      DPRINTF("\n");
    }

    DPRINTF("\n Top left corner of matrix B: \n");
    for (i=0; i<min(k,6); i++) {
      for (j=0; j<min(n,6); j++) {
        DPRINTF("%12.0f", B[j+i*n]);
      }
      DPRINTF("\n");
    }
    
    DPRINTF("\n Top left corner of matrix C: \n");
    for (i=0; i<min(m,6); i++) {
      for (j=0; j<min(n,6); j++) {
        DPRINTF("%12.5G", C[j+i*n]);
      }
      DPRINTF("\n");
    }

    DPRINTF("\n Deallocating memory \n\n");
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);

    DPRINTF(" Example completed. \n\n");
    return 0;
}


int main()
{
  bench_dgemm();
  bench_stream_triad();
  return 0;
}
