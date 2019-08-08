#ifndef __SLISTSTACK_H
#define __SLISTSTACK_H

#include "slist.h"

typedef slist sliststack;

sliststack *createsliststack();
void sliststackpush(sliststack *stack,slist *new);
void* sliststackpop(sliststack *stack);

sliststack *createsliststack()
{
  initslist(ret,0);
  return (sliststack*)ret;
}

void sliststackpush(sliststack *stack,slist *new)
{
  initslist(padding,0);
  appendslist(padding,stack);
  slistreplace(padding,new);
}

void* sliststackpop(sliststack *stack)
{
  slist *target=slist_last(stack);
  if(target==stack) return NULL;
  void* ret=target->key;
  slist_del(target);
  return ret;
}

void *getsliststacktop(sliststack *stack)
{
  slist *target=slist_last(stack);
  if(target==stack) return NULL;
  void* ret=target->key;
  return ret;
}
#endif
