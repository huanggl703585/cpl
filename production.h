#ifndef __PRODUCTION_H
#define __PRODUCTION_H

#include "slist.h"
#include "dynamicarray.h"
#include "pbodyunit.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct productionbody productionbody;
struct productionbody{
  pbody *body;
  int cnt;
  pbodyunit *unit;
  re_node *retree;
  struct list_head list;
};

#define prod_for_each_prodbody(pbpos,prod)			\
  list_for_each_entry(pbpos,&(prod->productionbody->list),list)

#define prodbodyfirst(prodbody)					\
  list_first_entry(&(prodbody->list),productionbody,list)

#define prodbodyfirstpbody(prodbody)			\
  list_first_entry(&(prodbody->body->list),pbody,list)

#define prodbodynext(prodbody)			\
  list_next_entry(prodbody,list)

#define appendprodbody(prodbody,elem)	\
  _appendprodbody(prodbody,elem);		\
  prodbody->cnt++

#define appendprodbodyterminal(prodbody,elem)	\
  _appendprodbody(prodbody,'\'');			\
  _appendprodbody(prodbody,elem);			\
  _appendprodbody(prodbody,'\'');			\
  prodbody->cnt++

#define _appendprodbody(prodbody,elem)		\
  appendpbody(prodbody->body,elem)

typedef struct production production;
struct production{
  int head;
  int cnt;
  re_node *retree;
  productionbody *productionbody;
};

#define productionappend(prod,prodbody)				\
  listaddtail(&(prodbody->list),&(prod->productionbody->list));	\
  prod->cnt++

#define productiondrop(prod,prodbody)		\
  listdrop(&(prodbody->list));			\
  prod->cnt--

#define appendprodrange(prod,lower,upper)				\
  do{									\
    for(int __i=lower;__i<=upper;__i++){				\
      productionbody *__pbpos=createprodbodylinkprod(prod);		\
      appendprodbodyterminal(__pbpos,__i);				\
    }									\
  }while(0)

productionbody *createprodbody();
productionbody *createprodbodylinkprod(production *prod);
production *createproduction(int head);
productionbody *pbodyunitcreateprodbody(pbodyunit *u);
void prodbodyappendpbodyunitlist(productionbody *prodbody,pbodyunit *start);
void prodbodyappendpbodyunit(productionbody *prodbody,pbodyunit *u);

productionbody *createprodbody()
{
  productionbody *ret=(productionbody*)malloc(sizeof(productionbody));
  createpbody((ret->body));
  list_init(ret->list);
  ret->cnt=0;
  ret->unit=createpbodyunit();
  return ret;
}

productionbody *createprodbodylinkprod(production *prod)
{
  productionbody *ret=createprodbody();
  productionappend(prod,ret);
  return ret;
}

production *createproduction(int head)
{
  production *ret=(production*)malloc(sizeof(production));
  ret->head=head;
  ret->productionbody=createprodbody();
  ret->cnt=0;
  //ret->unit=createpbodyunit();
  return ret;
}

productionbody *pbodyunitcreateprodbody(pbodyunit *u)
{
  productionbody *ret=createprodbody();
  ret->unit=u;
  return ret;
}

//range: (start,0)
void prodbodyappendpbodyunitlist(productionbody *prodbody,pbodyunit *start)
{
  pbodyunit *listhead=prodbody->unit;
  for(pbodyunit *pos=pbodyunitnext(start);pos->type!=0;pos=pbodyunitnext(pos)){
    pbodyunit *copy=_pbodyunitcopy(pos);
    pbodyunitappend(copy,listhead);
  }
}

void prodbodyappendpbodyunit(productionbody *prodbody,pbodyunit *u)
{
  pbodyunit *listhead=prodbody->unit;
  pbodyunit *copy=_pbodyunitcopy(u);
  pbodyunitappend(copy,listhead);
}

//=====================================
//20190812 FINISH
void prodbodypbodytounit(productionbody *prodbody);
void prodelimateparenthese(production *prod);
void _doprodelimateor(production* prod,productionbody *prodbody);
void prodelimateor(production *prod);

void prodbodypbodytounit(productionbody *prodbody)
{
  pbody *head=prodbody->body;
  pbodyunit *res=getunitbypbody(head,head);
  prodbody->unit=res;
}

void prodelimateparenthese(production *prod)
{
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunitelimateparenthese(pbpos->unit);
  }
}

void _doprodelimateor(production* prod,productionbody *prodbody)
{
  pbodyunit *u=prodbody->unit;
  pbodyunit *iterone=pbodyunitnext(u);
  pbodyunit *itertwo=pbodyunitnext(iterone);
  pbodyunit *tmpiterone=iterone,*tmpitertwo=itertwo;

  do{
    iterone=tmpiterone;
    itertwo=tmpitertwo;

    productionbody *newprodbody=createprodbodylinkprod(prod);
    prodbodyappendpbodyunit(newprodbody,iterone);

    tmpiterone=pbodyunitnext(itertwo);
    tmpitertwo=pbodyunitnext(tmpiterone);
  }while(itertwo!=u);
  productiondrop(prod,prodbody);
}

void prodelimateor(production *prod)
{
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *u=pbpos->unit;
    if(_testpbodyunitelimateor(u))
      _doprodelimateor(prod,pbpos);
  }
}

//====================re_node
void prodbuildretree(production *prod);
void prodbodybuildretree(productionbody *prodbody);

void prodbuildretree(production *prod)
{
  int head=prod->head;
  int size=prod->cnt;
  re_node *retreearr[size];
  int pt=0;
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    prodbodybuildretree(pbpos);
    retreearr[pt++]=pbpos->retree;
  }
  
  int haverecursion[size];
  int recursionnum;
  if((recursionnum=retreetestrecursion(retreearr,head,haverecursion,size))){
    re_node *norecursion[size-recursionnum];
    int tmppt=0;
    for(int i=0;i<size;i++)
      if(haverecursion[i]==0)
	norecursion[tmppt++]=retreearr[i];
    re_node *righttree=concatenateretree(norecursion,tmppt,RE_CAT);
    for(int i=0;i<size;i++)
      if(haverecursion[i]==1)
	retreereducerecursion(retreearr[i],head,righttree);
  }
  
  prod->retree=concatenateretree(retreearr,size,RE_OR);
  //printretree(prod->retree);
  //printf("\n");
}

void prodbodybuildretree(productionbody *prodbody)
{
  prodbody->retree=pbodyunitbuildretree(prodbody->unit);
  //printretree(prodbody->retree);
  //printf("\n");
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
