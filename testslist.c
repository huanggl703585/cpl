#include "sliststack.h"

int main()
{
  slist_init(l,0);
  slist_init(new,0);
  slist_init(l1,4);
  slist_init(l2,5);
  slist_init(l3,6);
  slist_init(l4,7);
  slist_init(l5,8);
  slist_append(l1,l);
  slist_append(l2,l);
  slist_append(l3,l);
  slist_append(l4,new);
  slist_append(l5,new);

  sliststack *stack=createSliststack();
  sliststackPush(stack,l);
  sliststackPush(stack,new);
  
  void *res;
  while((res=sliststackPop(stack))!=NULL){
    int ret=(int)(res);
    printf("%d ",ret);
  }
}
