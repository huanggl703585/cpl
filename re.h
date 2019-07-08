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

#define isleave(node) (node->left==NULL && node->right==NULL)

struct re_node{
  union{
    re_symbol *leave;
    re_operator *node;
  }key;
  int index;
  int nullable;
  set* firstpos;
  set* lastpos;
  struct re_node *left;
  struct re_node *right;
};

#define isnode(node) (node->left!=NULL || node->right!=NULL)


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
re_node* createrenode(int isleave,void *key);
re_node* reseqtotree(re_seq *seq);

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

re_node *createrenode(int isleave,void *key)
{
  re_node *ret=(re_node*)malloc(sizeof(re_node));
  ret->left=ret->right=NULL;
  if(isleave)
    ret->key.leave=(re_symbol*)key;
  else
    ret->key.node=(re_operator*)key;
  ret->firstpos=ret->lastpos=NULL;
  return ret;
}

//a left-lean tree, leave is symbol, node is operator
re_node* reseqtotree(re_seq *seq)
{
  re_symbol *curoperand;
  re_node *curnode;

  int indexcnt=1;
  curoperand=list_next_entry(seq->operands,list);
  curnode=createrenode(1,curoperand);
  curnode->index=indexcnt++;

  re_operator *oper=list_next_entry(seq->operator,list);
  do{
    re_node *rightnode=NULL,*parent=NULL;
    if(oper->operator != STAR){  //operator is binary operator
      curoperand=list_next_entry(curoperand,list);
      rightnode=createrenode(1,curoperand);
      rightnode->index=indexcnt++;
    }
    parent=createrenode(0,oper);
    parent->left=curnode;
    parent->right=rightnode;
    //printf("%d %d\n",curnode->key.leave->index,parent->key.node->operator);
    curnode=parent;

  }while((oper=list_next_entry(oper,list))!=seq->operator);
  return curnode;
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

//=====================for test/print
void travelresymbol(re_symbol *sym);
void travelretree(re_node *tree);

void travelresymbol(re_symbol *sym)
{
  re_symbol *iter=sym;
  do{
    printf("%d \n",iter->index);
  }while((iter=list_next_entry(iter,list))!=sym);
}

//post order traversal
void travelretree(re_node *tree)
{
  if(isnode(tree)){
    if(tree->left!=NULL)
      travelretree(tree->left);
    if(tree->right!=NULL)
      travelretree(tree->right);
    printf("%d \n",tree->key.node->operator);
  }
  else
    printf("%d \n",tree->key.leave->index);
}

#endif
