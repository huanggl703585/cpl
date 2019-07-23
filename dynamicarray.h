#ifndef __DYNAMICARRAY_H
#define __DYNAMICARRAY_H

#include <stdlib.h>
#include <stdio.h>

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

darray* createdarray(int initsize,int factor);
int insertdarray(darray* arr,void* value);
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
  

darray* createdarray(int initsize,int factor)
{
  darray *ret=(darray*)malloc(sizeof(darray));
  ret->pt=0;
  ret->size=initsize;
  ret->factor=factor;
  ret->array=(void**)malloc(sizeof(void*)*initsize);
}

int insertdarray(darray* arr,void *value)
{
  if(arr->pt==arr->size){
    arr->size *= arr->factor;
    arr->array=(void**)realloc(arr->array,sizeof(void*)*(arr->size));
  }
  arr->array[arr->pt++]=value;
}

#endif
