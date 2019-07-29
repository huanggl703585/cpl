#ifndef __SYMBOLATTR_H
#define __SYMBOLATTR_H

#include "graph.h"
#include "production.h"
#include "mapinput.h"
#include "re_exp.h"

#define NONTERMINAL1 1
#define NONTERMINAL2 2
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

symbolattr *createsymbolattr(int index);

symbolattr *createsymbolattr(int index)
{
  symbolattr *ret=(symbolattr*)malloc(sizeof(symbolattr));
  ret->attr.prod=NULL;
  ret->node=createnode(index);
  return ret;
}

/*
  non-terminal set type
  terminal set mapper
*/
int ntsymbolsetattr(symbolattr *attr);
int tsymbolappendmapper(symbolattr *attr,int map);

int ntsymbolsetattr(symbolattr *attr)
{
  production *ppos=attr->attr.prod;
  return 1;
}
#endif
