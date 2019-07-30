#ifndef __PRODUCTION_H
#define __PRODUCTION_H

#include "slist.h"
#include "dynamicarray.h"
#include "re_exp.h"
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

#define getpbodykey(body)			\
  (int)(body->key)				

#define getpbodynext(body)			\
  list_next_entry(body,list)

typedef struct productionbody productionbody;
struct productionbody{
  pbody *body;
  int cnt;
  struct list_head list;
};

#define productionbodyfirst(prodbody)		\
  list_first_entry(&(prodbody->list),productionbody,list)

#define prodbodyfirstelem(prodbody)		\
  list_first_entry(&(prodbody->body->list),pbody,list)

#define prodhaveleftrecursion(prod,prodbody) ({	\
  int __head=prod->head;			\
  pbody *__pos=prodbodyfirstelem(prodbody);	\
  int __elem=(int)(__pos->key);			\
  int __res=(__elem==__head);			\
  __res;})

typedef struct production production;
struct production{
  int head;
  int cnt;
  productionbody *productionbody;
};

#define productionappend(prod,prodbody)				\
  listaddtail(&(prodbody->list),&(prod->productionbody->list));	\
  prod->cnt++

#define appendprodbody(prodbody,elem)		\
  appendpbody(prodbody->body,elem);		\
  prodbody->cnt++

#define appendprodrange(prod,lower,upper)			\
  do{								\
    for(int __i=lower;__i<=upper;__i++){			\
      productionbody *__pbpos=createproductionbody(prod);	\
      appendprodbody(__pbpos,__i);				\
      __pbpos->cnt++;						\
    }								\
    prod->cnt+=(upper-lower+1);					\
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
  ret->cnt=0;
  productionappend(prod,ret);
  return ret;
}

production *createproduction(int head)
{
  production *ret=(production*)malloc(sizeof(production));
  ret->head=head;
  ret->productionbody=_createproductionbody();
  ret->cnt=0;
  return ret;
}

//================================

#define ispunctuate(x)				\
  (x=='(' || x==')' || x=='|')

re_exp *productiontoreexp(production *prod);
re_exp *_productiontoreexp(productionbody *prodbody);

re_exp *productiontoreexp(production *prod)
{
  if(prod->cnt==1){  //don't need parentheses
    productionbody *pbpos=productionbodyfirst(prod->productionbody);
    return _productiontoreexp(pbpos);
  }
  else{
    re_exp *ret=createreexp();
    int cnt=0;
    productionbody *pbpos;
    list_for_each_entry(pbpos,&(prod->productionbody->list),list){
      re_exp *tmp=_productiontoreexp(pbpos);
      reexpaddparentheses(tmp);
      listappend(&(ret->list),&(tmp->list));
      free(tmp);
      if(++cnt != prod->cnt){
	re_exp *tmpor=createreexp();
	tmpor->type=OPERATOR;
	tmpor->id=OR;
	listaddtail(&(tmpor->list),&(ret->list));
      }
    }
    return ret;
  }
}

re_exp *_productiontoreexp(productionbody *prodbody)
{
  re_exp *ret=createreexp();
  pbody *pos;
  list_for_each_entry(pos,&(prodbody->body->list),list){
    int key=(int)(pos->key);
    if(ispunctuate(key))
      reexpappend(ret,OPERATOR,key);
    else
      reexpappend(ret,OPERAND,key);
  }
  return ret;
}

//====================
//a non-terminal, each production contains only a terminal symbol
int prodisterminalset(production *prod);

int prodisterminalset(production *prod)
{
  productionbody *pbpos;
  list_for_each_entry(pbpos,&(prod->productionbody->list),list){
    pbody *tmp;
    //just has single member, and the member is a terminal
    int cnt=0;
    list_for_each_entry(tmp,&(pbpos->body->list),list){
      if((cnt++)==1) return 0;
      int key=(int)(tmp->key);
      if(key>127) return 0;
    }
  }
  return 1;
}

//pbody unit : if next symbol is '(' ,unit is segment between two parentheses
//             else, is the next symbol
#define pbodygetnextunit(body,head,res,len)	\
  do{						\
    pbody *pos=list_next_entry(body,list);	\
    int key=(int)(pos->key);			\
    if(key!-'('){				\
      res=pos;					\
      len=1;					\
    }						\
    else{					\
      len=1;					\
      while(key!=')'){				\
	pos=list_next_entry(pos,list);		\
	len++:					\
      }						\
      body=pos;					\
    }						\
  }while(0)

#define pbodycmpunit(body,head,dest,len) ({		\
  int res;						\
  if(len==1){						\
    body=list_next_entry(body,list);			\
    if(body==head)					\
      res=0;						\
    else{						\
      int key=(int)(body->key);				\
      int destkey=(int)(dest->key);			\
      res=(key==destkey);				\
    }							\
  }							\
  else{							\
    int tmp=len;					\
    body=list_next_entry(body,list);			\
    int key,destkey;					\
    while(tmp--){					\
      if(body==head)					\
	res=0;						\
      else{						\
	key=getpbodykey(body);				\
	destkey=getpbodykey(dest);			\
	body=getpbodynext(body);			\
	dest=getpbodynext(dest);			\
      }							\
    }							\
  }							\
  res;})
    
  

//longest common prefix
#define prodfindlcp(prod,resname)			\
  do{							\
    pbody resname[prod->cnt];				\
    int mark[prod->cnt];				\
    productionbody *prodbody=prod->productionbody;	\
    for(int i=0;i<prod->cnt;i++){			\
      prodbody=list_next_entry(prodbody,list);		\
      resname[i]=prodbody->body;			\
      mark[i]=1;					\
    }							\
    
  }while(0)

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
