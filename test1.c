#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  for(int i=0,j=2;i<10;j++,i=j+1){
    printf("%d %d\n",i,j);
  }
}
