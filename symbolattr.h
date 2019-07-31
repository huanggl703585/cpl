#ifndef __SYMBOLATTR_H
#define __SYMBOLATTR_H

#include "graph.h"
#include "production.h"
#include "mapinput.h"
#include "re_exp.h"

#define NONTERMINAL  1
#define TERMINALSET  2
#define TERMINAL     3

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
  int isset=prodisterminalset(prod);
  if(isset) attr->type=TERMINALSET;
  else attr->type=NONTERMINAL;
}
#endif
