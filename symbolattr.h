#ifndef __SYMBOLATTR_H
#define __SYMBOLATTR_H

#include "set.h"
#include "graph.h"
#include "production.h"
#include "mapinput.h"
#include "re_exp.h"

#include <assert.h>

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
  darray *forecastlist;
};

symbolattr *createsymbolattr(int id);

symbolattr *createsymbolattr(int id)
{
  symbolattr *ret=(symbolattr*)malloc(sizeof(symbolattr));
  ret->attr.prod=createproduction(id);
  return ret;
}

/*
  non-terminal set type
  terminal set mapper
*//*
void symbolsetattr(symbolattr *attr);
void _printsymbolattr(symbolattr *attr);

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

void _printsymbolattr(symbolattr *attr)
{
  int type=attr->type;
  switch(type){
  case TERMINAL: {printf("TERMINAL\n");break;}
  case TERMINALSET: {printf("TERMINALSET\n");break;}
  case TERMINALSEQ: {printf("TERMINALSEQ\n");break;}
  case NONTERMINAL: {printf("NONTERMINAL\n");break;}
  default:assert(0);
  } 
}
  */
//==============================
void _prodinunit(symbolattr *attr);
void _printtablepunit(symbolattr *attr);
void _elimateparenthese(symbolattr *attr);
void _elimateor(symbolattr *attr);
void _extractleftlcp(symbolattr *attr);

void _prodinunit(symbolattr *attr)
{
  production *prod=attr->attr.prod;
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    prodbodypbodytounit(pbpos);
  }
}

void _printtablepunit(symbolattr *attr)
{
  production *prod=attr->attr.prod;
  printf("prodcnt %d\n",prod->cnt);
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *unit=pbpos->unit;
    printpbodyunit(unit);
    printf("\n");
  }
}

void _elimateparenthese(symbolattr *attr)
{
  prodelimateparenthese(attr->attr.prod);
}

void _elimateor(symbolattr *attr)
{
  prodelimateor(attr->attr.prod);
}

#endif
