#ifndef __PRODUCTION_H
#define __PRODUCTION_H

#include "slist.h"
#include "dynamicarray.h"
#include "re_exp.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//we assume the the head of pbody is 0(NULL)  
typedef slist pbody;

#define pbody_for_each(pos,listhead)		\
  slist_for_each(pos,listhead)

#define createpbody(pos) 					\
  do{									\
    initslist(__newpbody,NULL);						\
    pos=__newpbody;							\
  }while(0)

void appendpbody(pbody *listhead,int elem)
{
  initslist(tmp,elem);
  appendslist(tmp,listhead);
}
/*
#define appendpbody(listhead,elem)		\
  do{						\
     initslist(__newpbody,elem);		\
     appendslist(__newpbody,listhead);		\
  }while(0)
*/
#define getpbodykey(body)			\
  (int)(body->key)				

#define getpbodynext(body)			\
  list_next_entry(body,list)

void pbodyappendlistwithlen(pbody *listhead,pbody *newlist,int len)
{
  int tmp=len;						
  pbody *pos=newlist;						
  while(tmp--){						
    pos=getpbodynext(pos);					
    int key=getpbodykey(pos);					
    appendpbody(listhead,key);				
  }
}

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
  appendpbody(prodbody->body,elem);		\
  prodbody->cnt++

#define appendprodrange(prod,lower,upper)			\
  do{								\
    for(int __i=lower;__i<=upper;__i++){			\
      productionbody *__pbpos=createproductionbody(prod);	\
      appendprodbody(__pbpos,__i);				\
    }								\
    prod->cnt+=(upper-lower+1);					\
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

//pbody unit : if next symbol is '(' ,unit is segment between two parentheses
//             else, is the next symbol
//body will move forward to the tail of unit
#define pbodygetnextunit(body,head,res,len)		\
  do{							\
    res=body;						\
    body=list_next_entry(body,list);			\
    int key=(int)(body->key);				\
    len=1;						\
    if(key=='('){					\
      while(key!=')'){					\
	body=list_next_entry(body,list);		\
	key=getpbodykey(body);				\
	len++;						\
      }							\
    }							\
  }while(0)

int pbodyunitequal(pbody *u1,int ulen1,pbody *u2,int ulen2)
{
  if(ulen1!=ulen2) return 0;
  pbody *tmp1=u1,*tmp2=u2;
  int key1,key2;
  int tmp=ulen1;
  while(tmp--){
    tmp1=getpbodynext(tmp1);
    tmp2=getpbodynext(tmp2);
    key1=getpbodykey(tmp1);
    key2=getpbodykey(tmp2);
    if(key1!=key2) return 0;
  }
  return 1;
}
    
void printpbodyunit(pbody *body,int len){
  printf("len %d\t",len);			
  pbody* pos=getpbodynext(body);		
  int tmp=len;				
  while(tmp--){				
    int key=getpbodykey(pos);			
    printf("%d ",key);			
    pos=getpbodynext(pos);			
  }						
  printf("\n");				
}

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
    pbody *origin[_prod->cnt];						\
    createpbody(_lcp);							\
    productionbody *prodbody=_prod->productionbody;			\
    for(int i=0;i<_prod->cnt;i++){					\
      prodbody=list_next_entry(prodbody,list);				\
      _res[i]=prodbody->body;						\
      origin[i]=prodbody->body;						\
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
      _res[i]=origin[i];						\
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
