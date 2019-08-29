#ifndef __SLISTSTACK_H
#define __SLISTSTACK_H

#include "slist.h"

typedef slist sliststack;

sliststack *createSliststack();
void sliststackPush(sliststack *stack,slist *new);
void* slistslistPop(sliststack *stack);
void *getSliststackTop(sliststack *stack);

sliststack *createSliststack()
{
  slist_init(ret,0);
  return (sliststack*)ret;
}

void sliststackPush(sliststack *stack,slist *new)
{
  slist_init(padding,0);
  slist_append(padding,stack);
  slist_replace(padding,new);
}

void* sliststackPop(sliststack *stack)
{
  slist *target=slist_last(stack);
  if(target==stack) return NULL;
  void* ret=target->key;
  slist_drop(target);
  return ret;
}

void *getSliststackTop(sliststack *stack)
{
  slist *target=slist_last(stack);
  if(target==stack) return NULL;
  void* ret=target->key;
  return ret;
}
#endif
