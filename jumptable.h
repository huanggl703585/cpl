#ifndef __JUMPTABLE_H
#define __JUMPTABLE_H

#include "jumptable_link.h"
#include <stdlib.h>

/*
  20190709
  now we just use base & next the two arrays
  different from four arrays default, base, next, check
  because of the difficulty of implements

  INCOMPLETE
*/

typedef struct jumptable jumptable;

struct jumptable{
  int statenum;
  int jumpnum;
  int *base;
  int *next;
};

jumptable* buildjumptable(jumptable_link *jtable);

jumptable* buildjumptable(jumptable_link *jtable)
{
  jumptable *ret=(jumptable*)malloc(sizeof(jumptable));
  ret->statenum=jtable->statecnt+1;
  ret->jumpnum=getjumpnum(jtable);
  ret->base=(int*)malloc(sizeof(int)*(ret->statenum));
  ret->next=(int*)malloc(sizeof(int)*(ret->jumpnum));
}



#endif
