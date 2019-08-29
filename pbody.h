#ifndef __PBODY_H
#define __PBODY_H

#include "slist.h"

//we assume the the head of pbody is 0(NULL)  
typedef slist pbody;

#define pbody_for_each(pos,listhead)		\
  slist_for_each(pos,listhead)

#define createPbody(pos) 					\
  do{									\
    slist_init(__newpbody,NULL);					\
    pos=__newpbody;							\
  }while(0)

#define appendPbody(listhead,elem)		\
  do{						\
     slist_init(__newpbody,elem);		\
     slist_append(__newpbody,listhead);		\
  }while(0)

#define getPbodyKey(body)			\
  (int)(body->key)				

#define getPbodyNext(body)			\
  list_next_entry(body,list)

#define getPbodyPrev(body)			\
  list_last_entry(body,list)

#define pbodyDrop(body)				\
  slist_drop(body)

void pbodyElimateParenthese(pbody *pos);

void pbodyElimateParenthese(pbody *pos)
{
  pbody *iter=pos;
  int key;
  while((key=getPbodyKey(iter))!=')'){
    iter=getPbodyNext(iter);
  }
  pbodyDrop(pos);
  pbodyDrop(iter);
}

#endif
