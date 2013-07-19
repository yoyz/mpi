/*
 * gflops.c
 *
 * Copyright (c) 2011-1012 Peyrard Johann <peyrard.johann@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY LUDOVIC SAUGE ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NORITOSHI MASUICHI BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include<mpi.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

long long max1=100000;
long long max2=1000;

double stoptime(void) 
{
  struct timeval t;
  gettimeofday(&t,NULL);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}



double addmul()
{
  float  mul1=1.1,mul2=1.2,mul3=1.3,mul4=1.4,mul5=1.5,mul6=1.6,mul7=1.7,mul8=1.8;
  float  mul=3.14;
  long long i,j;
  for(i=0; i< max1*max2 ; i++) 
	{
	  mul1*=mul;  
	  mul2*=mul;  
	  mul3*=mul;  
	  mul4*=mul; 
	  mul5*=mul;  
	  mul6*=mul;  
	  mul7*=mul;  
	  mul8*=mul;       
	}
  return  mul1+mul2+mul3+mul4+mul5+mul6+mul7+mul8;
}

int main(int argc, char** argv) 
{
  double t;
  double x;
  int size, rank;
  char hostname[1024];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


  hostname[1023] = '\0';
  gethostname(hostname, 1023);
  t=stoptime();
  x=addmul();
  t=stoptime()-t;
  printf("rank: %.4d\thost: %s\tgflops:\t %.3f s, %.3f Gflops, rank: %.4d res=%f\n",
	 rank,
	 hostname,
	 t,
	 (double)max1*max2*8/t/1e9,
	 rank,
	 x);

  MPI_Finalize();

  return 0;
}