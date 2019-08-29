#ifndef __MAPINPUT_H
#define __MAPINPUT_H

//map input char to dfa's alphaset
//for example, map all lower char to a number 
//i will use it to implement input set
//then simplify the dfa('s size)

#include "slist.h"

typedef slist charmapper;

#define initCharmapper(name,_key)			       \
  slist_init(name,_key)

#define charmapperAppend(listhead,_key)				\
  do{								\
    initCharmapper(__tmp,_key);					\
    slist_append(__tmp,listhead);				\
  }while(0)

#define for_each_charmapper(pos,listhead)		\
  list_for_each_entry(pos,&(listhead->list),list)

charmapper *createCharmapper(int self);
void printCharmapper(charmapper *mapper);

charmapper *createCharmapper(int self)
{
  initCharmapper(ret,NULL);
  charmapperAppend(ret,self);
  return ret;
}

void printCharmapper(charmapper *mapper)
{
  slist_print(mapper);
}
#endif
