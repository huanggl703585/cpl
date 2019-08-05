#ifndef __MAPINPUT_H
#define __MAPINPUT_H

//map input char to dfa's alphaset
//for example, map all lower char to a number 
//i will use it to implement input set
//then simplify the dfa('s size)

#include "slist.h"

typedef slist charmapper;

#define initcharmapper(name,_key)			       \
  initslist(name,_key)

#define charmapperappend(listhead,_key)				\
  do{								\
    initcharmapper(__tmp,_key);					\
    appendslist(__tmp,listhead);				\
  }while(0)

#define travelmapper(pos,listhead)		\
  list_for_each_entry(pos,&(listhead->list),list)

charmapper *createcharmapper(int self)
{
  initcharmapper(ret,NULL);
  //charmapperappend(ret,self);
  return ret;
}
#endif
