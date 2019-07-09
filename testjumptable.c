#include "jumptable_link.h"
#include <stdio.h>

int main()
{
  jumptable_link *jtable=createjumptablelink();
  jumptableadd(jtable,1,3,5);
  jumptableadd(jtable,1,2,4);
  jumptableadd(jtable,4,2,3);
  printf("\n");
  printf("%d %d %d %d\n",jumptablefind(jtable,1,3),jumptablefind(jtable,1,2),jumptablefind(jtable,4,2),jumptablefind(jtable,2,3));
  printf("%d\n",jtable->statecnt);
  printf("%d\n",getjumpsum(jtable));
}
