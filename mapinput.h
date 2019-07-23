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

#define appendcharmapper(listhead,_key)				\
  do{								\
    initcharmapper(__tmp,_key);					\
    appendslist(listhead,__tmp);				\
  }while(0)

#endif
