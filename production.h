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

#define prodbodyFirst(prodbody)					\
  list_first_entry(&(prodbody->list),productionbody,list)

#define prodbodyFirstPbody(prodbody)			\
  list_first_entry(&(prodbody->body->list),pbody,list)

#define prodbodyNext(prodbody)			\
  list_next_entry(prodbody,list)

#define appendProdbody(prodbody,elem)	\
  _appendProdbody(prodbody,elem);		\
  prodbody->cnt++

#define appendProdbodyOr(prodbody)		\
  appendProdbody(prodbody,'|')

#define appendProdbodyTerminal(prodbody,elem)	\
  _appendProdbody(prodbody,'\'');			\
  _appendProdbody(prodbody,elem);			\
  _appendProdbody(prodbody,'\'');			\
  prodbody->cnt++

#define _appendProdbody(prodbody,elem)		\
  appendPbody(prodbody->body,elem)

typedef struct production production;
struct production{
  int head;
  int cnt;
  re_node *retree;
  productionbody *productionbody;
};

#define productionAppend(prod,prodbody)				\
  listaddtail(&(prodbody->list),&(prod->productionbody->list));	\
  prod->cnt++

#define productionDrop(prod,prodbody)		\
  listdrop(&(prodbody->list));			\
  prod->cnt--

#define appendProdRange(prod,lower,upper)				\
  do{									\
    for(int __i=lower;__i<=upper;__i++){				\
      productionbody *__pbpos=createProdbodyLinkProd(prod);	\
      appendProdbodyTerminal(__pbpos,__i);				\
    }									\
  }while(0)

productionbody *createProdbody();
productionbody *createProdbodyLinkProd(production *prod);
production *createProduction(int head);
productionbody *pbodyunitCreateProdbody(pbodyunit *u);
void prodAppendEmptyProdbody(production *prod);
void prodbodyAppendPbodyunitList(productionbody *prodbody,pbodyunit *start);
void prodbodyAppendPbodyunit(productionbody *prodbody,pbodyunit *u);

productionbody *createProdbody()
{
  productionbody *ret=(productionbody*)malloc(sizeof(productionbody));
  createPbody((ret->body));
  list_init(ret->list);
  ret->cnt=0;
  ret->unit=createPbodyunit();
  return ret;
}

productionbody *createProdbodyLinkProd(production *prod)
{
  productionbody *ret=createProdbody();
  productionAppend(prod,ret);
  return ret;
}

production *createProduction(int head)
{
  production *ret=(production*)malloc(sizeof(production));
  ret->head=head;
  ret->productionbody=createProdbody();
  ret->cnt=0;
  //ret->unit=createPbodyunit();
  return ret;
}

productionbody *pbodyunitCreateProdbody(pbodyunit *u)
{
  productionbody *ret=createProdbody();
  ret->unit=u;
  return ret;
}

void prodAppendEmptyProdbody(production *prod)
{
  productionbody *emptypb=createProdbodyLinkProd(prod);
  pbodyunit *empty=createPbodyunitEmpty();
  prodbodyAppendPbodyunit(emptypb,empty);
}

//range: (start,0)
void prodbodyAppendPbodyunitList(productionbody *prodbody,pbodyunit *start)
{
  pbodyunit *listhead=prodbody->unit;
  int cnt=0;
  for(pbodyunit *pos=pbodyunitNext(start);pos->type!=0;pos=pbodyunitNext(pos)){
    pbodyunit *copy=_pbodyunitCopy(pos);
    pbodyunitAppend(copy,listhead);
    cnt++;
  }
  prodbody->cnt=cnt;
}

void prodbodyAppendPbodyunit(productionbody *prodbody,pbodyunit *u)
{
  pbodyunit *listhead=prodbody->unit;
  pbodyunit *copy=_pbodyunitCopy(u);
  pbodyunitAppend(copy,listhead);
  prodbody->cnt++;
}

//=====================================
//20190812 FINISH
void prodbodyPbodyToUnit(productionbody *prodbody);
void prodElimateParenthese(production *prod);
void _doProdElimateOr(production* prod,productionbody *prodbody);
void prodElimateOr(production *prod);

void prodbodyPbodyToUnit(productionbody *prodbody)
{
  pbody *head=prodbody->body;
  pbodyunit *res=getUnitByPbody(head,head);
  //printPbodyunit(res);
  prodbody->unit=res;
}

void prodElimateParenthese(production *prod)
{
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunitElimateParenthese(pbpos->unit);
  }
}

void _doProdElimateOr(production* prod,productionbody *prodbody)
{
  pbodyunit *u=prodbody->unit;
  pbodyunit *iterone=pbodyunitNext(u);
  pbodyunit *itertwo=pbodyunitNext(iterone);
  pbodyunit *tmpiterone=iterone,*tmpitertwo=itertwo;

  do{
    iterone=tmpiterone;
    itertwo=tmpitertwo;

    productionbody *newprodbody=createProdbodyLinkProd(prod);
    prodbodyAppendPbodyunit(newprodbody,iterone);

    tmpiterone=pbodyunitNext(itertwo);
    tmpitertwo=pbodyunitNext(tmpiterone);
  }while(itertwo!=u);
  productionDrop(prod,prodbody);
}

void prodElimateOr(production *prod)
{
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *u=pbpos->unit;
    if(_testPbodyunitElimateOr(u))
      _doProdElimateOr(prod,pbpos);
  }
}

//====================re_node
void prodBuildRetree(production *prod);
void prodbodyBuildRetree(productionbody *prodbody);

void prodBuildRetree(production *prod)
{
  int head=prod->head;
  int size=prod->cnt;
  re_node *retreearr[size];
  int pt=0;
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    prodbodyBuildRetree(pbpos);
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

void prodbodyBuildRetree(productionbody *prodbody)
{
  prodbody->retree=pbodyunitBuildRetree(prodbody->unit);
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
    slist_print(tmp);
  } 
}
#endif
