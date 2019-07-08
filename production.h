#ifndef __PRODUCTION_H
#define __PRODUCTION_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


int fd;
char *filename="cgrammer.txt";
void readfile()
{
  fd=open(filename,O_RDWR,S_IRWXU);
}

typedef struct production production;
struct production{
  int head;
  int *seq;
};


#endif
