#ifndef __TXTREADER_H
#define __TXTREADER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "doublebuf.h"
//TODO 
//IF BLOCKSIZE SETS TO 24, IT WILL CAUSE ERROR
//IT SEEMS THAT SWITCH BUF MULTIPLE TIME WILL CAUSE ERROR
#define BLOCKSIZE 24

typedef struct reader reader;
typedef struct txtsrc txtsrc;

struct txtsrc{
  int fd;
  int offset;
  int blocksize;
};

struct reader{
  txtsrc *src;
  doublebuf *dbuf;
};

int filltxt(char *buf,void *arg);

int filltxt(char *buf,void *arg)
{
  txtsrc *src=(txtsrc*)arg;
  lseek(src->fd,src->offset,SEEK_SET);
  int ret=read(src->fd,buf,src->blocksize);
  src->offset+=(src->blocksize);
  return 0;
}
    
txtsrc *createtxtsrc(char *path,int blocksize);
reader *createreader(char *path,int blocksize);

#define readone(reader) (readchar(reader->dbuf,reader->src))
#define acceptword(reader,recv,rsize) (readword(reader->dbuf,recv,rsize)) 

txtsrc *createtxtsrc(char *path,int blocksize)
{
  txtsrc *ret=(txtsrc*)malloc(sizeof(txtsrc));
  ret->fd=open(path,O_RDONLY);
  if(ret->fd==-1) return NULL;
  ret->offset=0;
  ret->blocksize=blocksize;
  return ret;
}

reader *createreader(char *path,int blocksize)
{
  reader *ret=(reader*)malloc(sizeof(reader));
  ret->src=createtxtsrc(path,blocksize);
  if(ret->src==NULL) return NULL;
  ret->dbuf=createdoublebuf(blocksize,filltxt,(void*)(ret->src));
  if(ret->dbuf==NULL) return NULL;

  return ret;
}


#endif
