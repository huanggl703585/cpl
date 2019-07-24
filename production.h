#ifndef __PRODUCTION_H
#define __PRODUCTION_H

#include "slist.h"
#include "dynamicarray.h"
#include <stdlib.h>

  
typedef slist pbody;

#define createpbody(pos) 					\
  do{									\
    initslist(__newpbody,NULL);						\
    pos=__newpbody;							\
  }while(0)

#define appendpbody(listhead,elem)		\
  do{						\
    initslist(__newpbody,elem);			\
    appendslist(__newpbody,listhead);		\
  }while(0)

typedef struct productionbody productionbody;
struct productionbody{
  pbody *body;
  struct list_head list;
};

typedef struct production production;
struct production{
  int head;
  productionbody *productionbody;
};

#define productionappend(prod,prodbody)		\
  listaddtail(&(prodbody->list),&(prod->productionbody->list))

#define appendprodbody(prodbody,elem)		\
  appendpbody(prodbody->body,elem)

#define appendprodrange(prod,lower,upper)			\
  do{								\
    for(int __i=lower;__i<=upper;__i++){			\
      productionbody *__pbpos=createproductionbody(prod);	\
      appendprodbody(__pbpos,__i);				\
    }								\
  }while(0)

productionbody *_createproductionbody();
productionbody *createproductionbody(production *prod);
production *createproduction(int head);

productionbody *_createproductionbody()
{
  productionbody *ret=(productionbody*)malloc(sizeof(productionbody));
  createpbody((ret->body));
  list_init(ret->list);
  return ret;
}

productionbody *createproductionbody(production *prod)
{
  productionbody *ret=_createproductionbody();
  productionappend(prod,ret);
  return ret;
}

production *createproduction(int head)
{
  production *ret=(production*)malloc(sizeof(production));
  ret->head=head;
  ret->productionbody=_createproductionbody();
  return ret;
}

//====================print/test
void printproduction(production *prod);

void printproduction(production *prod)
{
  printf("head %d\n",prod->head);
  productionbody *pbpos;
  list_for_each_entry(pbpos,&(prod->productionbody->list),list){
    printf("body ");
    pbody *tmp=pbpos->body;
    printslist(tmp);
  } 
}
#endif
