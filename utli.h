#ifndef __UTLI_H
#define __UTLI_H

#include <string.h>

#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)<(y)?(y):(x))

#define upperdiv(x,y) (((x)+(y)-1)/(y))

#define offsetof(TYPE,MEMBER)			\
  ((size_t)&((TYPE*)0)->MEMBER)

#define container_of(ptr,type,member) ({		\
  const typeof(((type *)0)->member) *__mptr=(ptr);	\
  (type*)((char*)__mptr-offsetof(type,member));})

int min3(int a,int b,int c);
int _getstr(char *pt); //get the offset of string start from pt

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
#endif
