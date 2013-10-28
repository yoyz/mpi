#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(char * argc,char **argv)
{
  int8_t * buff;
  FILE   * RFD;
  FILE   * WFD;

  int      rindex=0;
  int      windex=0;
  int      rcount=0;
  int      wcount=0;

  buff=malloc(sizeof(int8_t)*1024*1024);

  if (argc<2) { printf("Error\n"); exit(1); }
  printf("%s\n",argv[0]);
  printf("%s\n",argv[1]);
  printf("%s\n",argv[2]);
  RFD=fopen(argv[1],"r+");
  WFD=fopen(argv[2],"w+");
  while (rcount=fread(buff,1024*1024,sizeof(int8_t),RFD))
    {
      //rindex+=(rcount*1024*1024);
      wcount=fwrite(buff,1024*1024,sizeof(int8_t),WFD);
    }

  exit(0);
}

