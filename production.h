#ifndef __PRODUCTION_H
#define __PRODUCTION_H

#include "slist.h"
#include "dynamicarray.h"
#include "re_exp.h"
#include "pbodyunit.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct productionbody productionbody;
struct productionbody{
  pbody *body;
  int cnt;
  pbodyunit *unit;
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

void prodbodyappendpbodyunit(productionbody *prodbody,pbodyunit *u)
{
  pbodyunit *listhead=prodbody->unit;
  pbodyunitappend(u,listhead);
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
/*
//================================

#define ispunctuate(x)				\
  (x=='(' || x==')' || x=='|')

re_exp *productiontoreexp(production *prod,int head);
re_exp *_productiontoreexp(productionbody *prodbody,int head);

re_exp *productiontoreexp(production *prod,int head)
{
  if(prod->cnt==1){  //don't need parentheses
    productionbody *pbpos=productionbodyfirst(prod->productionbody);
    return _productiontoreexp(pbpos,head);
  }
  else{
    re_exp *ret=createreexp();
    int cnt=0;
    productionbody *pbpos;
    list_for_each_entry(pbpos,&(prod->productionbody->list),list){
      re_exp *tmp=_productiontoreexp(pbpos,head);
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

re_exp *_productiontoreexp(productionbody *prodbody,int head)
{
  re_exp *ret=createreexp();
  pbody *pos;
  int isfirst=1;
  list_for_each_entry(pos,&(prodbody->body->list),list){
    int key=(int)(pos->key);
    if(ispunctuate(key))
      reexpappendoperator(ret,key);
    else{
      if(key!=head){
	if(isfirst) isfirst=0;
	else reexpappendoperator(ret,CAT);
	reexpappendoperand(ret,key,head);
      }
      else{
	reexpaddparentheses(ret);
	reexpappendoperator(ret,STAR);
      }
    }
  }
  return ret;
}

//====================
int prodhasnonterminal(production *prod);
int prodisterminalset(production *prod);

int prodhasnonterminal(production *prod)
{
  productionbody *pbpos;
  list_for_each_entry(pbpos,&(prod->productionbody->list),list){
    pbody *tmp;
    list_for_each_entry(tmp,&(pbpos->body->list),list){
      int key=(int)(tmp->key);
      if(key>=127)
	return 1;
    }
  }
  return 0;
}

int prodisterminalset(production *prod)
{
  productionbody *pbpos;
  list_for_each_entry(pbpos,&(prod->productionbody->list),list){
    pbody *tmp;
    //just has single member, and the member is a terminal
    int cnt=0;
    list_for_each_entry(tmp,&(pbpos->body->list),list){
      if((cnt++)==1) return 0;
    }
  }
  return 1;
}

//====================left recursion
#define prodhaveleftrecursion(prod,prodbody) ({	\
  int __head=prod->head;			\
  pbody *__pos=prodbodyfirstelem(prodbody);	\
  int __elem=(int)(__pos->key);			\
  int __res=(__elem==__head);			\
  __res;})
*/
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
