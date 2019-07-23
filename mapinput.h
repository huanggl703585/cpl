#ifndef __MAPINPUT_H
#define __MAPINPUT_H

//map input char to dfa's alphaset
//for example, map all lower char to a number 
//i will use it to implement input set
//then simplify the dfa('s size)

#include "list.h"

#define SINGLECHAR 1
#define SETFILTER  2

typedef struct charmapper charmapper;
struct charmapper{
  int type;
  int priority;
  union{
    char singlechar;
    set* setfilter;
  }filter;
  int output;
  struct list_head chain;
};

#define initcharmapper(name,_type,_priority,_filter,_output)   \
  charmapper *name=(charmapper*)malloc(sizeof(charmapper));    \
  name->type=_type;					       \
  name->priority=_priority;				       \
  name->filter=_filter;					       \
  name->output=_output;					       \
  list_init(name->chain)

#define addcharmapper(new,list)					\
  list_insert_order((list),chain,priority,(new),intcmp_reverse)

int mapinput(charmapper** mapper,char input);

//we can use it in a iterater way
int mapinput(charmapper** mapper,char input)
{
  charmapper *head=*mapper;
  charmapper *curmapper;
  list_for_each_entry(curmapper,&(head->chain),chain){
    if(curmapper->type==SINGLECHAR){
      int res= (curmapper->filter.singlechar==input);
      *mapper=curmapper;
      return res;
    }
  if(findset(curmapper->filter.setfilter,input)){
      *mapper=curmapper;
      return curmapper->output;
    }
  }
  return 0;
}

#endif
