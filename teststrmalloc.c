#include <stdio.h>
#include <stdlib.h>
#include "strmalloc.h"
#include "random.h"

int main()
{
  int cnt=0;
  for(int i=0;i<300;i++){
    int num=getrandomnumber(1,101);
    char *pt=strmalloc(num);
    char *pt2=(char*)malloc(num);
    cnt+=num;
    printf("%d %p %p\n",num,pt,pt2);
  }
  printf("%d ",cnt);
}
