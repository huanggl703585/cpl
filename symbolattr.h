#ifndef __SYMBOLATTR_H
#define __SYMBOLATTR_H

#include "dfa.h"
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

//used in structual grammar
#define S_TERMINAL     5

typedef struct symbolattr symbolattr;
struct symbolattr{
  int type;
  union{
    production *prod;
    charmapper *mapper;
  }attr;
  dfa *dfa;

  set *first;
  set *follow;
  darray *forecastlist;
};

symbolattr terminalattr={.type=S_TERMINAL};

symbolattr *createSymbolAttr(int id);

symbolattr *createSymbolAttr(int id)
{
  symbolattr *ret=(symbolattr*)malloc(sizeof(symbolattr));
  ret->attr.prod=createproduction(id);
  return ret;
}

//==============================
void _prodInUnit(symbolattr *attr);
void _printTableUnit(symbolattr *attr);
void _elimateParenthese(symbolattr *attr);
void _elimateOr(symbolattr *attr);
void _extractLeftLcp(symbolattr *attr);

void _prodInUnit(symbolattr *attr)
{
  production *prod=attr->attr.prod;
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,prod){
    prodbodypbodytounit(pbpos);
  }
}

void _printTableUnit(symbolattr *attr)
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

void _elimateParenthese(symbolattr *attr)
{
  prodelimateparenthese(attr->attr.prod);
}

void _elimateOr(symbolattr *attr)
{
  prodelimateor(attr->attr.prod);
}

//========================symbol type
void _symbolSetType(symbolattr *attr);
void _printSymbolType(symbolattr *attr);

void _symbolSetType(symbolattr *attr)
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

void _printSymbolType(symbolattr *attr)
{
  int type=attr->type;
  switch(type){
  case S_HAVEEMPTY: {printf("S_HAVEEMPTY\n");break;}
  case S_TERMINALSET: {printf("S_TERMINALSET\n");break;}
  case S_TERMINALSEQ: {printf("S_TERMINALSEQ\n");break;}
  case S_NONTERMINAL: {printf("S_NONTERMINAL\n");break;}
  case S_TERMINAL: {printf("S_TERMINAL\n");break;}
  default:assert(0);
  } 
}
  
//===================re_tree
void _symboltableBuildRetree(symbolattr *attr);

void _symboltableBuildRetree(symbolattr *attr)
{
  production *prod=attr->attr.prod;
  if(attr->type!=S_TERMINALSET)
    prodbuildretree(prod);
  else
    prod->retree=createrenode(RE_OPERAND,prod->head);
}

#endif
