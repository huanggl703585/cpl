#include <stdio.h>
#include <stdlib.h>
#include "set.h"
#include "random.h"

int main()
{
  set *s1=NULL,*s2=NULL,*s3=NULL;;
  for(int i=0;i<5;i++){
    int a=getrandomnumber(0,40),b=getrandomnumber(30,70);
    insertset(s1,a);
    insertset(s2,b);
  }
  printset(s1);
  printset(s2);

  intersectset(&s3,s1);
  printset(s1);
  printset(s3);
}
