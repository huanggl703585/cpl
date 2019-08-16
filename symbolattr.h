#ifndef __SYMBOLATTR_H
#define __SYMBOLATTR_H

#include "set.h"
#include "graph.h"
#include "production.h"
#include "mapinput.h"

#include <assert.h>

//use 4 type to cover all nonterminal
//first : terminal-set, means all production is like a :: 'b', both useful to symbol and terminal
//sencond : other symbol that all its productions don't have any nonterminal
//third : other symbol
//forth : to the third kind of symbol that have empty production 
#define S_TERMINALSET  1
#define S_TERMINALSEQ  2
#define S_NONTERMINAL  3
#define S_HAVEEMPTY    4

struct symbolattr;
typedef struct symbolattr symbolattr;

struct symbolattr{
  int type;
  union{
    production *prod;
    charmapper *mapper;
  }attr;
  node *node;
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

//========================symbol type
void _symbolsettype(symbolattr *attr);
void _printsymboltype(symbolattr *attr);

void _symbolsettype(symbolattr *attr)
{
  production *prod=attr->attr.prod;
  productionbody *pbpos;
  int isterminalset=1;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *list=pbpos->unit;
    int res=pbodyunitlisttype(list);
    if(isterminalset==1 && res!=2)
      isterminalset=0;
    if(res==1){
      attr->type=S_HAVEEMPTY;
      return ;
    }
    if(res==3){
      attr->type=S_NONTERMINAL;
      return ;
    }
  }
  if(isterminalset==1)
    attr->type=S_TERMINALSET;
  else 
    attr->type=S_TERMINALSEQ;
}

void _printsymboltype(symbolattr *attr)
{
  int type=attr->type;
  switch(type){
  case S_HAVEEMPTY: {printf("S_HAVEEMPTY\n");break;}
  case S_TERMINALSET: {printf("S_TERMINALSET\n");break;}
  case S_TERMINALSEQ: {printf("S_TERMINALSEQ\n");break;}
  case S_NONTERMINAL: {printf("S_NONTERMINAL\n");break;}
  default:assert(0);
  } 
}
  
//===================re_tree
void _symboltablebuildretree(symbolattr *attr)
{
  production *prod=attr->attr.prod;
  prodbuildretree(prod);
}
#endif
