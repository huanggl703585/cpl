#ifndef __RE_EXP_H
#define __RE_EXP_H

#include "re.h"
#include <stdlib.h>

#define OPERAND  1
#define OPERATOR 2

typedef struct re_exp re_exp;
struct re_exp{
  int type;
  int id;
  struct list_head list;
  //used for dfa's output
  int sign;
};

#define initreexp(name)				\
  re_exp name;					\
  list_init(name.list)				\
  
#define reexpaddparentheses(head)		\
  do{						\
    re_exp *__new1=createreexp();		\
    re_exp *__new2=createreexp();		\
    __new1->type=__new2->type=OPERATOR;		\
    __new1->id=LEFTPARTH;			\
    __new2->id=RIGHTPARTH;			\
    listadd(&(__new1->list),&(head->list));	\
    listaddtail(&(__new2->list),&(head->list));	\
  }while(0)

re_exp* createreexp();
void reexpappend(re_exp *head,int type,int id,int mark);
re_exp *reexpfind(re_exp *head,int type,int id);
void reexpreplace(re_exp *pos,re_exp *new);

re_exp* createreexp()
{
  re_exp *ret=(re_exp*)malloc(sizeof(re_exp));
  list_init(ret->list);
  return ret;
}

void reexpappendoperator(re_exp *head,int id)
{
  re_exp *new=createreexp();
  new->type=OPERATOR;
  new->id=id;
  listaddtail(&(new->list),&(head->list));
}

void reexpappendoperand(re_exp *head,int id,int mark)
{
  re_exp *new=createreexp();
  new->type=OPERAND;
  new->id=id;
  new->sign=mark;
  listaddtail(&(new->list),&(head->list));
}
re_exp *reexpfind(re_exp *head,int type,int id)
{
  re_exp *pos;
  list_for_each_entry(pos,&(head->list),list){
    if(pos->type==type && pos->id==id)
      return pos;
  }
  return NULL;
}

#define reexpreplacesignunchange(pos,new)	\
  listreplace(&(pos->list),&(new->list));


void reexpreplace(re_exp *pos,re_exp *new)
{
  int newsign=pos->sign;
  re_exp *p;
  list_for_each_entry(p,&(new->list),list){
    p->sign=newsign;
  }
  listreplace(&(pos->list),&(new->list));
}

//==============================print/test
void printreexp(re_exp *exp);

void printreexp(re_exp *exp)
{
  re_exp *pos;
  list_for_each_entry(pos,&(exp->list),list){
    if(pos->type==OPERATOR){
      _printreoperator(pos->id);
    }
    else
      printf("%d",pos->id);
  }
  printf("\n");
  list_for_each_entry(pos,&(exp->list),list){
    printf("%d ",pos->sign);
  }
  printf("\n");
}

#endif
