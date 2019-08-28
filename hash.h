#ifndef __HASH_H
#define __HASH_H

#include <limits.h>
#include <ctype.h>
#include "utli.h"
#include "list.h"
#define GOLDEN_RATIO_32 0x61C88647

unsigned int hash_generic(unsigned int val);
unsigned int strHash(char *z);

unsigned int getHashIndex(unsigned int inttype,char* strtype,int size);

unsigned int hash_generic(unsigned int val)
{
  return (GOLDEN_RATIO_32 * val)%UINT_MAX;
}

unsigned int strHash(char *z)
{
  unsigned int ret=0;
  char c;
  while((c=(unsigned char)*z++)!='\0'){
    ret+=tolower(c);
    ret=(ret*0x9e3779b1)%UINT_MAX;
  }
  return ret;
}

unsigned int getHashIndex(unsigned int inttype,char *strtype,int size)
{
  unsigned int index;
  if(strtype!=NULL)
    index=strHash(strtype)%(size);
  else index=hash_generic((unsigned int)inttype)%(size);
  //else index=inttype%size;
  return index;
}

#endif
