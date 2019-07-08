#ifndef __STRMALLOC_H
#define __STRMALLOC_H

#include <unistd.h>
#include "list.h"

unsigned int chunk_size=0;

char *strmalloc(size_t size);

struct malloc_chunk{
  int nextoffset;
  struct list_head chunklist;
  char chunk[];
};

struct malloc_chunk *curmallocchunk;

int initstrmalloc();

#define sure_strmallocinit()			\
  (chunk_size!=0 || initstrmalloc())

void addstrmallocchunk();

int initstrmalloc()
{
  chunk_size=sysconf(_SC_PAGESIZE);

  struct malloc_chunk *pt=sbrk(chunk_size);
  pt->nextoffset=sizeof(struct malloc_chunk);
  list_init(pt->chunklist);
  curmallocchunk=pt;

  return 1;
}

void addstrmallocchunk()
{
  struct malloc_chunk *pt=sbrk(chunk_size);
  pt->nextoffset=sizeof(struct malloc_chunk);
  list_init(pt->chunklist);
  listaddtail(&(pt->chunklist),&(curmallocchunk->chunklist));
  curmallocchunk=pt;
}

char *strmalloc(size_t size)
{
  sure_strmallocinit();

  if((chunk_size-curmallocchunk->nextoffset)<size){
    printf("\n\nadd new chunk\n");
    addstrmallocchunk();
    return strmalloc(size);
  }

  char *pt=(char*)((char*)curmallocchunk+curmallocchunk->nextoffset);
  curmallocchunk->nextoffset+=size;
  
  return pt;
}

#endif
