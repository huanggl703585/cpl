#ifndef __METAGRAMMAR_H
#define __METAGRAMMAR_H

#include "grammar.h"

//meta-grammar means that we use that grammar to read other grammar

grammar *metagrammar;

int buildmetagrammar();

int buildmetagrammar()
{
  metagrammar=(grammar*)malloc(sizeof(grammar));
  metagrammar->table=createsymboltable(symbol_table_basic_size,ASCII_BIAS);
  symboltable *table=metagrammar->table; //it is a alias, for brief code
  int array[256];
  int pt=0;
  bzero(array,256*sizeof(int));
  
  array[pt++]=insertsymboltable(table,"token",NULL);
  array[pt++]=insertsymboltable(table,"identifier",NULL);
  array[pt++]=insertsymboltable(table,"identifier-nondigit",NULL);
  array[pt++]=insertsymboltable(table,"digit",NULL);
  array[pt++]=insertsymboltable(table,"equivalence",NULL);
  metagrammar->start=array[0];

  symbolitem* spos;
  production* ppos;
  productionbody *pbpos;
  //token ::= identifier | equivalence | or
  spos=searchsymboltablebyid(table,array[0]);
  spos->attr->attr.prod=createproduction(array[0]);
  ppos=spos->attr->attr.prod;
  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,array[1]);
  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,array[4]);

  //identifier ::= identifier-nondigit
  //           ::= identifier identifier-nondigit
  //           ::= identifier digit
  spos=searchsymboltablebyid(table,array[1]);
  ppos=spos->attr->attr.prod=createproduction(array[1]);
  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,array[2]);
  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,array[1]);
  appendprodbody(pbpos,array[3]);
  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,array[1]);
  appendprodbody(pbpos,array[4]);
  
  //equivalence ::= ':' ':' '='
  spos=searchsymboltablebyid(table,array[4]);
  ppos=spos->attr->attr.prod=createproduction(array[4]);
  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,':');
  appendprodbody(pbpos,':');
  appendprodbody(pbpos,'=');
  
  //identifier-nondigit 
  spos=searchsymboltablebyid(table,array[2]);
  ppos=spos->attr->attr.prod=createproduction(array[2]);
  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,'_');
  appendprodrange(ppos,'0','9');
  appendprodrange(ppos,'a','z');
  appendprodrange(ppos,'A','Z');

  //digit
  spos=searchsymboltablebyid(table,array[3]);
  ppos=spos->attr->attr.prod=createproduction(array[3]);
  appendprodrange(ppos,'0','9');

  for(int i=0;i<5;i++){
    spos=searchsymboltablebyid(table,array[i]);
    printproduction(spos->attr->attr.prod);
  }
  return 1;
}

#endif
