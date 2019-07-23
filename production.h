#ifndef __PRODUCTION_H
#define __PRODUCTION_H

#include "slist.h"
#include "dynamicarray.h"
#include <stdlib.h>

  
typedef slist pbody;

//at start pos==NULL, and later, pos will be a list_head of pbody
#define createpbody(pos) 					\
  do{									\
    initslist(__newpbody,NULL);						\
    if(pos!=NULL){							\
      appendslist(__newpbody,((slist*)pos));				\
    }									\
    else{								\
      pos=__newpbody;							\
    }									\
  }while(0)

#define appendpbody(listhead,elem)		\
  do{						\
    initslist(__newpbody,elem);			\
    appendslist(__newpbody,listhead);		\
  }while(0)

//TODO
typedef struct production production;
struct production{
  int head;
  pbody *body;
  struct list_head list;
};

#endif
