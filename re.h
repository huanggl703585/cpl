#ifndef __RE_H
#define __RE_H

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct re_symbol re_symbol;
typedef struct re_operator re_operator;
typedef struct re_seq re_seq;
typedef struct re_node re_node;

#define RE_END_SYMBOL 0
#define RE_EMPTY_SYMBOL 1
struct re_symbol{
  int index;//index==0 means a terminal symbol '#'
  struct list_head list;

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

struct re_seq{
  int symnum;
  int opernum;
  re_symbol *operands;
  re_operator *operator;
};


/*
  create a regular_expression
  input: number of symbol, number of operand
  output: a regular_expression in a sequence
*/
re_seq* initreseq(int symnum,int opernum);

/*
  insert RE_END_SYMBOL in symbol_list
  insert CAT operand in operand_list
*/
void expandreseq(re_seq *seq);

re_symbol* initresymbol();
re_symbol* createresymbol(int index);
void insertresymbol(re_seq *seq,int index);
re_operator* initreoperator();
re_operator* createreoperator(int operator);
void insertreoperator(re_seq *seq,int operator);


re_operator* resymtooper(re_seq *seq,int symindex);

re_seq* initreseq(int symnum,int opernum)
{
  re_seq* ret=(re_seq*)malloc(sizeof(re_seq));
  ret->symnum=symnum;
  ret->opernum=opernum;
  ret->operands=initresymbol();
  ret->operator=initreoperator();
  return ret;
}

void expandreseq(re_seq *seq)
{
  seq->symnum++;
  seq->opernum++;
  insertresymbol(seq,RE_END_SYMBOL);
  insertreoperator(seq,CAT);
}

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
 
void insertresymbol(re_seq *seq,int index)
{
  re_symbol* new=createresymbol(index);
  listaddtail(&(new->list),&(seq->operands->list));
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

void insertreoperator(re_seq *seq,int operator)
{
  re_operator* new=createreoperator(operator);
  listaddtail(&(new->list),&(seq->operator->list));
}

//point to 'next', for example:
//symbol:   a    b      c
//operator:CAT STAR OR STAR
//point to the first non-STAR operator
re_operator* resymtooper(re_seq *seq,int symindex)
{
  re_operator *ret=seq->operator;
  for(int i=0;i<=symindex;i++){
    if(ret->operator==STAR)
      ret=list_next_entry(ret,list);
    ret=list_next_entry(ret,list);
  }
  return ret;
}
//=====================iterator

#define nextoperands(operands)			\
  (list_next_entry(operands,list))
#define nextoperator(operator)			\
  (list_next_entry(operator,list))

#define operandsend(iter)			\
  (iter->operands==iter->seq->operands)
#define operatorend(iter)			\
  (iter->operator==iter->seq->operator)
#define reiteratorend(iter)			\
  (operandsend(iter) && operatorend(iter))

typedef struct re_iterator re_iterator;
struct re_iterator{
  re_seq *seq;
  re_symbol *operands;
  re_operator *operator;
};

re_iterator *createreiterator(re_seq *seq);
re_symbol* getnextoperands(re_iterator *iter);
re_operator* getnextoperator(re_iterator *iter);

#define initreiterator(iter,seq)		\
  iter.seq=seq;					\
  iter.operands=nextoperands(seq->operands);	\
  iter.operator=nextoperator(seq->operator)

re_iterator *createreiterator(re_seq *seq)
{
  re_iterator *ret=(re_iterator*)malloc(sizeof(re_iterator));
  ret->seq=seq;
  ret->operands=nextoperands(seq->operands);
  ret->operator=nextoperator(seq->operator);
  return ret;
}

re_symbol* getnextoperands(re_iterator *iter)
{
  if(operandsend(iter))
    return NULL;
  re_symbol *ret=iter->operands;
  iter->operands=nextoperands(iter->operands);
  //printf("%d \n",ret->index);

  return ret;
}

re_operator* getnextoperator(re_iterator *iter)
{
  if(operatorend(iter))
    return NULL;
  re_operator *ret=iter->operator;
  iter->operator=nextoperator(iter->operator);
 
  return ret;
}
//=====================for test/print
void travelresymbol(re_symbol *sym);

void travelresymbol(re_symbol *sym)
{
  re_symbol *iter=sym;
  do{
    printf("%d \n",iter->index);
  }while((iter=list_next_entry(iter,list))!=sym);
}

#endif
