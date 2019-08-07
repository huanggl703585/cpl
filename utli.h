#ifndef __UTLI_H
#define __UTLI_H

#include <stdio.h>
#include <string.h>

#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)<(y)?(y):(x))

#define intequal(x,y) ((int)x==(int)y)
#define intcmp(x,y) ((((int)x)>((int)y))?1:((((int)x)==((int)y))?0:-1))
#define intcmp_reverse(x,y) (((x)<(y))?1:(((x)==(y))?0:-1))

#define upperdiv(x,y) (((x)+(y)-1)/(y))

#define keepinterval(minval,maxval,newval) {	\
    minval=(minval>newval)?newval:minval;	\
    maxval=(maxval<newval)?newval:maxval;	\
  }
  
#define intiszero(x) (x==0)

#define arrayhaszero(head,size,iszero) ({		\
    int __i,__res=-1;					\
    for(__i=0;__i<size;__i++){				\
      if(iszero(head[__i])){				\
	__res=__i;					\
	break;						\
      }							\
    }							\
    __res;})						

#define arrayhasnonzero(head,size,iszero) ({		\
    int __i,__res=-1;					\
    for(__i=0;__i<size;__i++){				\
      if(!(iszero(head[__i]))){				\
	__res=__i;					\
	break;						\
      }							\
    }							\
    __res;})						

#define printintarray(arr,size)			\
  do{						\
    int __i;					\
    for(__i=0;__i<size;__i++)			\
      printf("%d ",arr[__i]);			\
    printf("\n");				\
  }while(0)					

//in a array ,given a value, find whether has a same value in the array
#define arrayfindequal(arr,size,value,isequal)	({	\
  int __res=-1;						\
  for(int __i=0;__i<size;__i++)				\
    if(isequal(value,arr[__i])){			\
      __res=__i;					\
      break;						\
    }							\
  __res;})
  

#define offsetof(TYPE,MEMBER)			\
  ((size_t)&((TYPE*)0)->MEMBER)

#define container_of(ptr,type,member) ({		\
  const typeof(((type *)0)->member) *__mptr=(ptr);	\
  (type*)((char*)__mptr-offsetof(type,member));})

int funcintcmp(void *a,void *b)
{
  int _a=(int)a;
  int _b=(int)b;
  return intcmp(_a,_b);
}


int min3(int a,int b,int c);
int _getstr(char *pt); //get the offset of string start from pt
int arraysortinsert(int *arr,int size,int key);
int intarrayisequal(int *arr1,int *arr2,int len1,int len2);

int min3(int a,int b,int c)
{
  if(min(a,b)==a){
    if(min(a,c)==a)
      return a;
    else 
      return c;
  }
  else{
    if(min(b,c)==b)
      return b;
    else
      return c;
  }
}

int _getstr(char *pt)
{
  int cnt=0;
  char *temp=pt;
  while((*temp)==' ')
    temp++;
  while((*temp)!=' ' && (*temp)!='\n' && (*temp)!='\0'){
    temp++;
    cnt++;
  }
  return cnt;
}

//we always assume the unuse value is 0, insert key larger than 0
//TESTED
int arraysortinsert(int *arr,int size,int key)
{
  int mid,low=0,high=size-1;
  while(low<high){
    if(low+1==high){
      if(arr[low]!=0 && key>arr[low])
	{mid=high;break;}
      else
	{mid=low;break;}
    }
    mid=(low+high+1)/2;
    //printf("%d %d %d\n",low,high,mid);
    if(arr[mid]==0 || arr[mid]>key)
      high=mid;
    else if(arr[mid]==key)
      break;
    else
      low=mid;
  }
  //printf(" %d\n",mid);
  if(mid+1==size) 
    ;
  else
    memmove(&(arr[mid+1]),&(arr[mid]),sizeof(int)*(size-mid-1));
  arr[mid]=key;
}

//first: compare len ;second: compare int
int intarrayisequal(int *arr1,int *arr2,int len1,int len2)
{
  if(len1!=len2) return 0;
  int i;
  for(i=0;i<len1;i++){
    if(arr1[i]!=arr2[i])	   
      return 0;
  }
  return 1;
}

#endif
