#ifndef __RE_H
#define __RE_H

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct re_symbol re_symbol;
typedef struct re_operator re_operator;

#define RE_END_SYMBOL 0
#define RE_EMPTY_SYMBOL 1
struct re_symbol{
  int index;//index==0 means a terminal symbol '#'
  struct list_head list;
  //used for dfa's output
  int sign;
};

#define CAT  1
#define OR   2
#define STAR 3
#define LEFTPARTH 4
#define RIGHTPARTH 5

struct re_operator{
  int operator;
  struct list_head list;
};


re_symbol* initresymbol();
re_symbol* createresymbol(int index);
re_operator* initreoperator();
re_operator* createreoperator(int operator);

re_symbol* initresymbol()
{
  re_symbol *ret=(re_symbol*)malloc(sizeof(re_symbol));
  ret->index=0;
  list_init(ret->list);
  return ret;
}

re_symbol* createresymbol(int index)
{
  re_symbol* ret=(re_symbol*)malloc(sizeof(re_symbol));
  ret->index=index;
  list_init(ret->list);
}
 

re_operator* initreoperator()
{
  re_operator *ret=(re_operator*)malloc(sizeof(re_operator));
  ret->operator=-1;
  list_init(ret->list);
  return ret;
}

re_operator* createreoperator(int operator)
{ 
  re_operator* ret=(re_operator*)malloc(sizeof(re_operator));
  ret->operator=operator;
  list_init(ret->list);
}


//=====================for test/print
#define _printreoperator(index)			\
  do{						\
    switch(index){				\
    case STAR:{printf("*");break;}		\
    case CAT: {printf("o");break;}		\
    case OR:  {printf("|");break;}		\
    case LEFTPARTH: {printf("(");break;}	\
    case RIGHTPARTH: {printf(")");break;}	\
    }						\
  } while(0)

void travelresymbol(re_symbol *sym);
void travelreoperator(re_operator *oper);

void travelresymbol(re_symbol *sym)
{
  re_symbol *iter=sym;
  do{
    printf("%d ",iter->index);
  }while((iter=list_next_entry(iter,list))!=sym);
  printf("\n");
}

void travelreoperator(re_operator *oper)
{
  re_operator *iter=oper;
  do{
    printf("%d ",iter->operator);
  }while((iter=list_next_entry(iter,list))!=oper);
  printf("\n");
}
#endif
