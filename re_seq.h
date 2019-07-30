#ifndef __RE_SEQ_H
#define __RE_SEQ_H

#include "re_exp.h"

typedef struct re_seq re_seq;
struct re_seq{
  int symnum;
  int opernum;
  re_symbol *operands;
  re_operator *operator;
};

re_seq *createreseq();
/*
  insert RE_END_SYMBOL in symbol_list
  insert LEFTPARTH & RIGHTPARTH
  insert CAT operand in operand_list
*/
void expandreseq(re_seq *seq);


#define addoperatorhead(seq,_operator)			\
  do{							\
    re_operator *__tmp=createreoperator(_operator);	\
    listadd(&(__tmp->list),&(seq->operator->list));	\
    seq->opernum++;					\
  }while(0)

void insertresymbol(re_seq *seq,int index);
void insertreoperator(re_seq *seq,int operator);
re_operator* resymtooper(re_seq *seq,int symindex);

re_seq *createreseq()
{
  re_seq *ret=(re_seq*)malloc(sizeof(re_seq));
  ret->symnum=ret->opernum=0;
  ret->operands=initresymbol();
  ret->operator=initreoperator();
  return ret;
}

void expandreseq(re_seq *seq)
{
  seq->symnum++;
  seq->opernum++;
  insertresymbol(seq,RE_END_SYMBOL);
  addoperatorhead(seq,LEFTPARTH);
  insertreoperator(seq,CAT);
  insertreoperator(seq,RIGHTPARTH);
}

void insertresymbol(re_seq *seq,int index)
{
  re_symbol* new=createresymbol(index);
  listaddtail(&(new->list),&(seq->operands->list));
  seq->symnum++;
}


void insertreoperator(re_seq *seq,int operator)
{
  re_operator* new=createreoperator(operator);
  listaddtail(&(new->list),&(seq->operator->list));
  seq->opernum++;
}


//point to 'next', for example:
//symbol:   a    b      c
//operator:CAT STAR OR STAR
//point to the first non-STAR operator
re_operator* resymtooper(re_seq *seq,int symindex)
{
  re_operator *ret=seq->operator;
  for(int i=0;i<=symindex;i++){
    if(ret->operator&(STAR|LEFTPARTH|RIGHTPARTH))
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

//============================================
re_seq *reexptoreseq(re_exp *exp);

re_seq *reexptoreseq(re_exp *exp)
{
  re_seq *ret=createreseq();
  re_exp *pos;
  list_for_each_entry(pos,&(exp->list),list){
    if(pos->type==OPERATOR)
      insertreoperator(ret,pos->id);
    else
      insertresymbol(ret,pos->id);
  }
  return ret;
}
#endif
