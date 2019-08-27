#ifndef __METAGRAMMAR_H
#define __METAGRAMMAR_H

#include "tokenizer.h"
#include "production.h"
#include "grammar.h"

//#include "LL1.h"

//meta-grammar means that we use that grammar to read other grammar
char *path="#cgrammer.txt";

grammar meta_lexcial_grammar;
grammar meta_structual_grammar;

void buildMetagrammar();
void buildMetaLexcialGrammar();
void buildMetaStructualGrammar();

//UNTEST
kvpair *copytokentable(grammar *src,symboltable *dst);
kvpair *_copytokentable(symboltable *src,pbodyunit *unit,symboltable *dst);

void buildMetagrammar()
{
  buildMetaLexcialGrammar();
  buildMetaStructualGrammar();
}

void buildMetaLexcialGrammar()
{
  meta_lexcial_grammar.table=createsymboltable(symbol_table_basic_size,ASCII_BIAS);
  symboltable *table=meta_lexcial_grammar.table;
  int array[256];
  int pt=0;
  bzero(array,256*sizeof(int));
  
  array[pt++]=insertsymboltable(table,"token",NULL);
  array[pt++]=insertsymboltable(table,"identifier",NULL);
  array[pt++]=insertsymboltable(table,"terminal",NULL);
  array[pt++]=insertsymboltable(table,"equivalence",NULL);
  array[pt++]=insertsymboltable(table,"identifier-nondigit",NULL);
  array[pt++]=insertsymboltable(table,"digit",NULL);
  array[pt++]=insertsymboltable(table,"alpha",NULL);
  meta_lexcial_grammar.start=array[0];
  
  symbolitem* spos;
  production* ppos;
  productionbody *pbpos;
  //token ::= identifier | equivalence | singlechar | '|' | '(' | ')'
  spos=searchsymboltablebyid(table,array[0]);
  spos->attr->attr.prod=createproduction(array[0]);
  ppos=spos->attr->attr.prod;
  pbpos=createprodbodylinkprod(ppos);
  appendprodbody(pbpos,array[1]);
  appendprodbodyor(pbpos);
  appendprodbody(pbpos,array[2]);
  appendprodbodyor(pbpos);
  appendprodbody(pbpos,array[3]);
  appendprodbodyor(pbpos);
  appendprodbodyterminal(pbpos,'|');
  appendprodbodyor(pbpos);
  appendprodbodyterminal(pbpos,'(');
  appendprodbodyor(pbpos);
  appendprodbodyterminal(pbpos,')');
  
  //identifier ::= identifier-nondigit
  //           ::= identifier identifier-nondigit
  //           ::= identifier digit
  spos=searchsymboltablebyid(table,array[1]);
  ppos=spos->attr->attr.prod=createproduction(array[1]);
  pbpos=createprodbodylinkprod(ppos);
  appendprodbody(pbpos,array[4]);
  pbpos=createprodbodylinkprod(ppos);
  appendprodbody(pbpos,array[1]);
  appendprodbody(pbpos,array[4]);
  pbpos=createprodbodylinkprod(ppos);
  appendprodbody(pbpos,array[1]);
  appendprodbody(pbpos,array[5]);

  //terminal ::= alpha | digit
  spos=searchsymboltablebyid(table,array[2]);
  ppos=spos->attr->attr.prod=createproduction(array[2]);
  pbpos=createprodbodylinkprod(ppos);
  appendprodbody(pbpos,array[5]);
  appendprodbodyor(pbpos);
  appendprodbody(pbpos,array[6]);

  //equivalence ::= ':' ':' '='
  spos=searchsymboltablebyid(table,array[3]);
  ppos=spos->attr->attr.prod=createproduction(array[3]);
  pbpos=createprodbodylinkprod(ppos);
  appendprodbodyterminal(pbpos,':');
  appendprodbodyterminal(pbpos,':');
  appendprodbodyterminal(pbpos,'=');

  //identifier-nondigit ::= '_' | alpha
  //when in debug: identifier-nondigit ::= ( '_' | alpha )
  spos=searchsymboltablebyid(table,array[4]);
  ppos=spos->attr->attr.prod=createproduction(array[4]);
  pbpos=createprodbodylinkprod(ppos);
  appendprodbody(pbpos,'(');
  appendprodbodyterminal(pbpos,'_');
  appendprodbodyor(pbpos);
  appendprodbody(pbpos,array[6]);
  appendprodbody(pbpos,')');

  //digit
  spos=searchsymboltablebyid(table,array[5]);
  ppos=spos->attr->attr.prod=createproduction(array[5]);
  appendprodrange(ppos,'0','9');
  
  //alpha
  spos=searchsymboltablebyid(table,array[6]);
  ppos=spos->attr->attr.prod=createproduction(array[6]);
  appendprodrange(ppos,'a','z');
  appendprodrange(ppos,'A','Z');

  reformproduction(table);
  //printtablepunit(table);
 
  symbolsettype(table);
  //printsymboltype(table);
  symbolsetmapper(table);
  //printsymbolmapper(table);
  symboltoposort(table);
  //printproductionwithname(table,1);
  re_node *tree=symboltablebuildretree(table,table->bias);
  //printretree(tree);
  dfa *dfa=createdfa(tree,tree->nodenum);
  //printdfa(dfa);
  tokenizer *tokenizer=createtokenizer(path,1024,1024);
  tokenizer->gtable=table;
  tokenizer->dfa=createdfainstance(dfa);
  
  _dotokenizer(tokenizer);
  //token *ttmp;
  //while((ttmp=gettoken(tokenizer))!=NULL)
  //  ;
  printtokenlistwithname(tokenizer);

  //buildfirst(table);
  //buildfollow(table,table->bias);
  //printfirstandfollow(table);

  //buildforecasttable(table);
  //printforecasttable(table);
}

void  buildMetaStructualGrammar()
{
  meta_structual_grammar.table=createsymboltable(symbol_table_basic_size,ASCII_BIAS);
  symboltable *table=meta_structual_grammar.table;
  int array[256];
  int pt=0;
  bzero(array,256*sizeof(int));
    /*
  array[pt++]=insertsymboltable(table,"start",NULL);
  array[pt++]=insertsymboltable(table,"production",NULL);
  array[pt++]=insertsymboltable(table,"head",NULL);
  array[pt++]=insertsymboltable(table,"body",NULL);
  meta_lexcial_grammar.start=array[0];

  symbolitem* spos;
  production* ppos;
  productionbody *pbpos;
  //start ::= production
  //      ::= start production
  spos=searchsymboltablebyid(table,array[0]);
  spos->attr->attr.prod=createproduction(array[0]);
  ppos=spos->attr->attr.prod;
  pbpos=createprodbodylinkprod(ppos);
  appendprodbody(pbpos,array[1]);
  appendprodbody(pbpos,'|');
  appendprodbody(pbpos,array[2]);
  appendprodbody(pbpos,'|');
  appendprodbody(pbpos,array[3]);
  appendprodbody(pbpos,'|');
  appendprodbodyterminal(pbpos,'|');
  appendprodbody(pbpos,'|');
  appendprodbodyterminal(pbpos,'(');
  appendprodbody(pbpos,'|');
  appendprodbodyterminal(pbpos,')');
    */
}

//return a map from src(key) to dst(value)
kvpair *copytokentable(grammar *src,symboltable *dst)
{
  kvpair *ret=createkvpairhead();
  symbolitem *start=searchsymboltablebyid(src->table,src->start);
  int newid=insertsymboltable(dst,start->name,&terminalattr);
  intkvpairinsert(ret,src->start,newid);
  
  production *srcprod=start->attr->attr.prod;
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,srcprod){
    pbodyunit *phead=pbpos->unit;
    pbodyunit *ppos;
    for_each_pbodyunit(ppos,phead){
      kvpair *pair=_copytokentable(src->table,ppos,dst);
      if(kvpairishead(pair))
	intkvpairlistinsert(ret,pair);
      else
	_intkvpairinsert(ret,pair);
    }
  }
  return ret;
}

kvpair *_copytokentable(symboltable *src,pbodyunit *unit,symboltable *dst)
{
  if(unit->type==P_TERMINAL || unit->type==P_NONTERMINAL){
    int id=unit->value.index;
    symbolitem *item=searchsymboltablebyid(src,id);
    int newid=insertsymboltable(dst,item->name,&terminalattr);
    kvpair *ret=createkvpair(id,newid);
    return ret;
  }
  if(unit->type==P_COMBINE){
    kvpair *ret=createkvpairhead();
    pbodyunit *head=unit->value.nest;
    pbodyunit *ppos;
    for_each_pbodyunit(ppos,head){
      kvpair *pair=_copytokentable(src,ppos,dst);
      _intkvpairinsert(ret,pair);
    }
    return ret;
  }
  return NULL;
}

#endif
