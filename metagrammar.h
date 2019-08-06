#ifndef __METAGRAMMAR_H
#define __METAGRAMMAR_H

#include "tokenizer.h"
#include "production.h"
#include "grammar.h"

#include "LL1.h"

//meta-grammar means that we use that grammar to read other grammar
char *path="cgrammer.txt";

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

  //re_exp *tmp=productiontoreexp(ppos);
  //printreexp(tmp);


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
  appendprodbody(pbpos,array[2]);

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

  
  
  //printproductionwithname(table);
  symbolsettype(table);
  extractleftlcp(table);
  elimateleftrecursion(table);
  symbolsettype(table);
  symbolsetmapper(table);
  symboltoposort(table);
  prodsettoreexp(table);
  //TODO
  //printreexpset(table);
  //printproductionwithname(table,2);
  
  int id=table->toposort[table->count-1];
  symbolitem *tokenitem=searchsymboltablebyid(table,id);
  re_seq *seq=reexptoreseq(tokenitem->attr->reexp);
  re_node *tree=buildtree(seq);
  //travelretree(tree);
  int nodenum=reseqgetnodenum(seq);
  //printf("nodenum %d ",nodenum);
  //printreseq(seq);
  dfa *dfa=createdfa(tree,nodenum);

  tokenizer *tokenizer=createtokenizer(path,1024,1024);
  tokenizer->gtable=table;
  tokenizer->dfa=createdfainstance(dfa);
  
  _dotokenizer(tokenizer);
  //token *ttmp;
  //while((ttmp=gettoken(tokenizer))!=NULL)
  //  ;
  //printtokenlist(tokenizer->tlist);

  buildfirst(table);
  buildfollow(table,table->bias);
  printfirstandfollow(table);
  return 1;
}

#endif
