#include "sliststack.h"

int main()
{
  initslist(l,0);
  initslist(new,0);
  initslist(l1,4);
  initslist(l2,5);
  initslist(l3,6);
  initslist(l4,7);
  initslist(l5,8);
  appendslist(l1,l);
  appendslist(l2,l);
  appendslist(l3,l);
  appendslist(l4,new);
  appendslist(l5,new);

  sliststack *stack=createsliststack();
  sliststackpush(stack,l);
  sliststackpush(stack,new);
  
  void *res;
  while((res=sliststackpop(stack))!=NULL){
    int ret=(int)(res);
    printf("%d ",ret);
  }
}
