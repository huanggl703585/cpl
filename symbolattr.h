#ifndef __SYMBOLATTR_H
#define __SYMBOLATTR_H

#include "set.h"
#include "graph.h"
#include "production.h"
#include "mapinput.h"
#include "re_exp.h"

#define NONTERMINAL  1
#define TERMINALSET  2
#define TERMINALSEQ  3
#define TERMINAL     4

struct symbolattr;
typedef struct symbolattr symbolattr;

struct symbolattr{
  int type;
  union{
    production *prod;
    charmapper *mapper;
  }attr;
  node *node;
  re_exp *reexp;
  set *first;
  set *follow;
};

symbolattr *createsymbolattr();

symbolattr *createsymbolattr()
{
  symbolattr *ret=(symbolattr*)malloc(sizeof(symbolattr));
  ret->attr.prod=NULL;
  return ret;
}

/*
  non-terminal set type
  terminal set mapper
*/
void symbolsetattr(symbolattr *attr);

void symbolsetattr(symbolattr *attr)
{
  production *prod=attr->attr.prod;
  int isnt=prodhasnonterminal(prod);
  if(isnt) 
    attr->type=NONTERMINAL;
  else if(prodisterminalset(prod))
    attr->type=TERMINALSET;
  else 
    attr->type=TERMINALSEQ;
}
#endif
