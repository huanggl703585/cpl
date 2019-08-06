#ifndef __SLIST_H
#define __SLIST_H

#include "list.h"

typedef struct slist slist;
struct slist{
  void *key;
  struct list_head list;
};

#define appendslist(new,chain)			\
  listaddtail(&(((slist*)new)->list),&(((slist*)chain)->list))

#define initslist(name,_key)			\
  slist *name=(slist*)malloc(sizeof(slist));	\
  name->key=(void*)_key;			\
  list_init(name->list)

#define printslist(listhead)			\
  do{							\
    slist* pos;				\
    list_for_each_entry(pos,&(((slist*)listhead)->list),list){ \
      printf("%d ",(int)(((slist*)pos)->key));				\
    }									\
    printf("\n");							\
  }while(0)								\
    
#define slist_for_each(pos,listhead)		\
  list_for_each_entry(pos,&(listhead->list),list)
#endif
