#ifndef __DYNAMICARRAY_H
#define __DYNAMICARRAY_H

#include <stdlib.h>
#include <stdio.h>

#include "kvpair.h"

typedef struct darray darray;
struct darray{
  void** array;
  int pt;
  int size;
  int factor;
};

#define printdarray(arr)			\
  do{						\
    for(int __i=0;__i<arr->pt;__i++)		\
      printf("%d ",(int)arr->array[__i]);	\
    printf("\n");				\
  }while(0)

#define insertdarray(darr,value)		\
  _insertdarray(darr,(void*)value)

darray* createdarray(int initsize,int factor);
void _insertdarray(darray* arr,void* value);

void sortinsertdarray(darray *arr,void *value,int (*cmp)(void*,void*));

#define sortfinddarray(darr,key,cmp)			\
  _sortfinddarray(darr,(void*)key,cmp)
//we assume that, the value of the darray is a kvpair, 
//argument key is the key of the kvpair, cmp function is used to compare key
void *_sortfinddarray(darray *arr,void *key,int (*cmp)(void *,void *));

#define finddarray(arr,index)			\
  (arr->array[index])
#define getdarraycnt(arr)			\
  (arr->pt)
#define valueindarray(arr,value,isequal) ({	\
      int __res=0;				\
      for(int __i=0;__i<arr->pt;__i++){		\
	if(isequal(arr->array[__i],value)){	\
	  __res=1;				\
	  break;				\
	}}					\
      __res;})					\
  
#define testexpanddarray(darr)			\
  do{						\
    if(darr->pt==darr->size){						\
      darr->size *= darr->factor;					\
      darr->array=(void**)realloc(darr->array,sizeof(void*)*(darr->size)); \
    }									\
  }while(0)

darray* createdarray(int initsize,int factor)
{
  darray *ret=(darray*)malloc(sizeof(darray));
  ret->pt=0;
  ret->size=initsize;
  ret->factor=factor;
  ret->array=(void**)malloc(sizeof(void*)*initsize);
}

void _insertdarray(darray* arr,void *value)
{
  testexpanddarray(arr);
  arr->array[arr->pt++]=value;
}

void sortinsertdarray(darray *arr,void *value,int (*cmp)(void *,void*))
{
  int i=0;
  for(i=0;i<arr->pt;i++){
    void *tmp=finddarray(arr,i);
    if(cmp(value,tmp)<0)
      break;
  }
  void *padding;
  insertdarray(arr,padding);
  int movenum=arr->pt-i-1;
  memmove(&(arr->array[i+1]),&(arr->array[i]),sizeof(void*)*movenum);
  arr->array[i]=value;
}

void *_sortfinddarray(darray *arr,void *key,int (*cmp)(void *,void *))
{
  int low=0,high=arr->pt;
  int mid;
  while(high>low){
    mid=(high+low)/2;
    //printf("%d %d %d\n",high,mid,low);
    void* object=finddarray(arr,mid);
    int cmpres=cmp(object,key);
    if(cmpres==0)
      return object;
    else if(cmpres<0)
      low=mid+1;
    else
      high=mid-1;
  }
  return NULL;
}
#endif
