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
  struct list_head list;
};

#define prodbody_for_each(pbpos,prod)		\
  list_for_each_entry(pbpos,&(prod->productionbody->list),list)

#define productionbodyfirst(prodbody)		\
  list_first_entry(&(prodbody->list),productionbody,list)

#define prodbodyfirstelem(prodbody)		\
  list_first_entry(&(prodbody->body->list),pbody,list)

#define prodbodynext(prodbody)			\
  list_next_entry(prodbody,list)


typedef struct production production;
struct production{
  int head;
  int cnt;
  productionbody *productionbody;
};

#define productionappend(prod,prodbody)				\
  listaddtail(&(prodbody->list),&(prod->productionbody->list));	\
  prod->cnt++

//TODO: how to free a productionbody?
#define productiondrop(prod,prodbody)		\
  listdel(&(prodbody->list));			\
  prod->cnt--;

#define appendprodbody(prodbody,elem)		\
  _appendprodbody(prodbody,elem);		\
  prodbody->cnt++

#define appendprodbodyterminal(prodbody,elem)	\
  _appendprodbody(prodbody,'\'');		\
  _appendprodbody(prodbody,elem);		\
  _appendprodbody(prodbody,'\'');		\
  prodbody->cnt++

#define _appendprodbody(prodbody,elem)		\
  appendpbody(prodbody->body,elem)

#define appendprodrange(prod,lower,upper)				\
  do{									\
    for(int __i=lower;__i<=upper;__i++){				\
      productionbody *__pbpos=createproductionbody(prod);		\
      appendprodbodyterminal(__pbpos,__i);				\
    }									\
    prod->cnt+=(upper-lower+1);						\
  }while(0)

productionbody *_createproductionbody();
productionbody *createproductionbody(production *prod);
production *createproduction(int head);
productionbody *createprodbodywithpbody(pbody *body,int cnt);

productionbody *_createproductionbody()
{
  productionbody *ret=(productionbody*)malloc(sizeof(productionbody));
  createpbody((ret->body));
  list_init(ret->list);
  ret->cnt=0;
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
  ret->cnt=0;
  return ret;
}

//re-build the pbody
productionbody *createprodbodywithpbody(pbody *body,int cnt)
{
  productionbody *ret=_createproductionbody();
  pbodyappendlistwithlen(ret->body,body,cnt);
  ret->cnt=cnt;
  return ret;
}

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
  
//========================
//disassemble a production with 'or'('|') into multiple production
//for example, a|b => a and another production b, here a & b is a pbody unit
//another example, (ab)|a -> ab and another production a, here we elimate parentheses
void dodisassembleor(production *prod);
void _dodisassembleor(production *prod,productionbody *prodbody);

void dodisassembleor(production *prod)
{
  productionbody *pbpos;
  prodbody_for_each(pbpos,prod){
    _dodisassembleor(prod,pbpos);
  }
}

void _dodisassembleor(production *prod,productionbody *prodbody)
{
  pbody *head=prodbody->body;
  pbody *iter=head;
  pbody *firstunit;
  pbody *secondunit;
  int firstlen,secondlen;
  
  int flag=1;
  do{
    pbodygetnextunit(iter,head,firstunit,firstlen);
    pbodygetnextunit(iter,head,secondunit,secondlen);
    
    pbody *test=getpbodynext(firstunit);
    int testkey=getpbodykey(test);

    pbody *secondnext=getpbodynext(secondunit);
    int key=getpbodykey(secondnext);

    printf("%d %d\n",testkey,key);
    /*if(secondlen==1 && key=='|'){
      //pbodyunittestelimateparenthese(firstunit,&firstlen);
      productionbody *tmp=createprodbodywithpbody(firstunit,firstlen);
      productionappend(prod,tmp);
      //pbodyunitdel(firstunit,firstlen);
    }
    else 
    flag=0;*/
    if(key==0) flag=0;
  }while(flag==1);
  printf("\n");
}

//========================lcp

//longest common prefix
//in:prod ; out: other
//mark is a int array, size is prod->cnt
//res is a pbody array, size if prod->cnt
//lcp is a pbody, it will init in the macro
//len is a int
//restlen is a int array, it just suitable for those mark[i]==1
#define prodfindlcp(_prod,_mark,_res,_lcp,_len,_restlen)		\
  int _mark[_prod->cnt];						\
  int _restlen[_prod->cnt];						\
  pbody *_res[_prod->cnt];						\
  pbody *_lcp;								\
  int _len=0;								\
  do{									\
    pbody *_origin[_prod->cnt];						\
    createpbody(_lcp);							\
    productionbody *prodbody=_prod->productionbody;			\
    for(int i=0;i<_prod->cnt;i++){					\
      prodbody=list_next_entry(prodbody,list);				\
      _res[i]=prodbody->body;						\
      _origin[i]=prodbody->body;					\
      _mark[i]=1;							\
      _restlen[i]=prodbody->cnt;					\
    }									\
    int flag=1;								\
    while(flag){							\
      pbody *unit[_prod->cnt];						\
      int unitlen[_prod->cnt];						\
      for(int i=0;i<_prod->cnt && _mark[i]==1;i++){			\
	pbodygetnextunit(_res[i],_origin[i],unit[i],unitlen[i]);	\
      }									\
      pbody *same;							\
      createpbody(same);						\
      int cmplen=0;							\
      int cflag=1;							\
      for(int i=0;i<_prod->cnt && _mark[i]==1 && cflag;i++){		\
	for(int j=i+1;j<_prod->cnt && _mark[j]==1 && cflag;j++){	\
	  if(listisempty(same->list)){					\
	    int cmp=pbodyunitequal(unit[i],unitlen[i],unit[j],unitlen[j]); \
	    if(cmp==1){							\
	      pbodyappendlistwithlen(same,unit[i],unitlen[i]);		\
	      cmplen=unitlen[i];					\
	      cflag=0;							\
	    }								\
	  }								\
	}								\
      }									\
      if(cflag==1)							\
	flag=0;								\
      else{								\
	pbodyappendlistwithlen(_lcp,same,cmplen);			\
	_len+=cmplen;							\
	for(int i=0;i<_prod->cnt && _mark[i]==1;i++){			\
	  int cmp=pbodyunitequal(same,cmplen,unit[i],unitlen[i]);	\
	  if(cmp==0)							\
	    _mark[i]=0;							\
	  else								\
	    _restlen[i]-=cmplen;					\
	}								\
      }									\
    }									\
    for(int i=0;i<_prod->cnt && _mark[i]==0;i++)			\
      _res[i]=_origin[i];						\
  }while(0)

//====================left recursion
#define prodhaveleftrecursion(prod,prodbody) ({	\
  int __head=prod->head;			\
  pbody *__pos=prodbodyfirstelem(prodbody);	\
  int __elem=(int)(__pos->key);			\
  int __res=(__elem==__head);			\
  __res;})

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
