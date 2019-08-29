#ifndef __METAGRAMMAR_H
#define __METAGRAMMAR_H

#include "tokenizer.h"
#include "production.h"
#include "grammar.h"

#include "LL1.h"

//meta-grammar means that we use that grammar to read other grammar
char *path="#cgrammer.txt";

grammar meta_lexcial_grammar;
grammar meta_structual_grammar;

void buildMetagrammar();
void buildMetaLexcialGrammar();
void buildMetaStructualGrammar();

//UNTEST
kvpair *copyTokentable(grammar *src,symboltable *dst);
kvpair *_copyTokentable(symboltable *src,pbodyunit *unit,symboltable *dst);

void buildMetagrammar()
{
  buildMetaLexcialGrammar();
  buildMetaStructualGrammar();
}

void buildMetaLexcialGrammar()
{
  meta_lexcial_grammar.table=createSymboltable(symbol_table_basic_size,ASCII_BIAS);
  symboltable *table=meta_lexcial_grammar.table;
  int array[256];
  int pt=0;
  bzero(array,256*sizeof(int));
  
  array[pt++]=insertSymboltable(table,"token",NULL);
  array[pt++]=insertSymboltable(table,"identifier",NULL);
  array[pt++]=insertSymboltable(table,"terminal",NULL);
  array[pt++]=insertSymboltable(table,"equivalence",NULL);
  array[pt++]=insertSymboltable(table,"identifier-nondigit",NULL);
  array[pt++]=insertSymboltable(table,"digit",NULL);
  array[pt++]=insertSymboltable(table,"alpha",NULL);
  meta_lexcial_grammar.start=array[0];
  
  symbolitem* spos;
  production* ppos;
  productionbody *pbpos;
  //token ::= identifier | equivalence | singlechar | '|' | '(' | ')'
  spos=searchSymboltableById(table,array[0]);
  spos->attr->attr.prod=createProduction(array[0]);
  ppos=spos->attr->attr.prod;
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,array[1]);
  appendProdbodyOr(pbpos);
  appendProdbody(pbpos,array[2]);
  appendProdbodyOr(pbpos);
  appendProdbody(pbpos,array[3]);
  appendProdbodyOr(pbpos);
  appendProdbodyTerminal(pbpos,'|');
  appendProdbodyOr(pbpos);
  appendProdbodyTerminal(pbpos,'(');
  appendProdbodyOr(pbpos);
  appendProdbodyTerminal(pbpos,')');

  //identifier ::= identifier-nondigit
  //           ::= identifier identifier-nondigit
  //           ::= identifier digit
  spos=searchSymboltableById(table,array[1]);
  ppos=spos->attr->attr.prod=createProduction(array[1]);
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,array[4]);
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,array[1]);
  appendProdbody(pbpos,array[4]);
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,array[1]);
  appendProdbody(pbpos,array[5]);

  //terminal ::= alpha | digit
  spos=searchSymboltableById(table,array[2]);
  ppos=spos->attr->attr.prod=createProduction(array[2]);
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,array[5]);
  appendProdbodyOr(pbpos);
  appendProdbody(pbpos,array[6]);

  //equivalence ::= ':' ':' '='
  spos=searchSymboltableById(table,array[3]);
  ppos=spos->attr->attr.prod=createProduction(array[3]);
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbodyTerminal(pbpos,':');
  appendProdbodyTerminal(pbpos,':');
  appendProdbodyTerminal(pbpos,'=');

  //identifier-nondigit ::= '_' | alpha
  //when in debug: identifier-nondigit ::= ( '_' | alpha )
  spos=searchSymboltableById(table,array[4]);
  ppos=spos->attr->attr.prod=createProduction(array[4]);
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,'(');
  appendProdbodyTerminal(pbpos,'_');
  appendProdbodyOr(pbpos);
  appendProdbody(pbpos,array[6]);
  appendProdbody(pbpos,')');

  //digit
  spos=searchSymboltableById(table,array[5]);
  ppos=spos->attr->attr.prod=createProduction(array[5]);
  appendProdRange(ppos,'0','9');
  
  //alpha
  spos=searchSymboltableById(table,array[6]);
  ppos=spos->attr->attr.prod=createProduction(array[6]);
  appendProdRange(ppos,'a','z');
  appendProdRange(ppos,'A','Z');

  reformLexcialProduction(table);
  //printTablePunit(table);
  
  re_node *tree=symboltableBuildRetree(table,table->bias);
  //printretree(tree);
  dfa *dfa=createdfa(tree,tree->nodenum);
  //printdfa(dfa);
  //tokenizer *tokenizer=createTokenizer(path,1024,1024);
  //tokenizer->gtable=table;
  //tokenizer->dfa=createdfainstance(dfa);
  
  //_doTokenizer(tokenizer);
  //token *ttmp;
  //while((ttmp=gettoken(tokenizer))!=NULL)
  //  ;
  //printTokenlistWithName(tokenizer);

  //buildfirst(table);
  //buildfollow(table,table->bias);
  //printfirstandfollow(table);

  //buildforecasttable(table);
  //printforecasttable(table);
}

void  buildMetaStructualGrammar()
{
  meta_structual_grammar.table=createSymboltable(symbol_table_basic_size,0);
  symboltable *ltable=meta_lexcial_grammar.table;
  symboltable *table=meta_structual_grammar.table;
  kvpair *map=copyTokentable(&meta_lexcial_grammar,table);

  int array[256];
  int pt=0;
  bzero(array,256*sizeof(int));
    
  array[pt++]=insertSymboltable(table,"start",NULL);
  array[pt++]=insertSymboltable(table,"production",NULL);
  array[pt++]=insertSymboltable(table,"head",NULL);
  array[pt++]=insertSymboltable(table,"body",NULL);
  meta_lexcial_grammar.start=array[0];

  symbolitem* spos;
  production* ppos;
  productionbody *pbpos;
  //start ::= production
  //      ::= start production
  spos=searchSymboltableById(table,array[0]);
  spos->attr->attr.prod=createProduction(array[0]);
  ppos=spos->attr->attr.prod;
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,array[0]);
  appendProdbody(pbpos,array[1]);
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,array[1]);

  //production ::= head equivalence body
  spos=searchSymboltableById(table,array[1]);
  spos->attr->attr.prod=createProduction(array[1]);
  ppos=spos->attr->attr.prod;
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbody(pbpos,array[2]);
  appendProdbodyByName(table,pbpos,"equivalence");
  appendProdbody(pbpos,array[3]);
  
  
  //head ::= identifier
  spos=searchSymboltableById(table,array[2]);
  spos->attr->attr.prod=createProduction(array[2]);
  ppos=spos->attr->attr.prod;
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbodyByName(table,pbpos,"identifier");
  
  
  //body ::= token body
  //     ::= token
  spos=searchSymboltableById(table,array[3]);
  spos->attr->attr.prod=createProduction(array[3]);
  ppos=spos->attr->attr.prod;
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbodyByName(table,pbpos,"token");
  appendProdbody(pbpos,array[3]);
  pbpos=createProdbodyLinkProd(ppos);
  appendProdbodyByName(table,pbpos,"token");

  int srcid=meta_lexcial_grammar.start;
  int dstid=intkvpairfind(map,srcid);
  spos=searchSymboltableById(table,dstid);
  spos->attr->attr.prod=createProduction(dstid);
  ppos=spos->attr->attr.prod;
  pbpos=createProdbodyLinkProd(ppos);
  kvpair *kvpos;
  //printf("%d %d\n",srcid,dstid);
  for_each_kvpair(kvpos,map){
    if((int)(kvpos->value)==dstid) continue;
    appendProdbody(pbpos,(int)(kvpos->value));
    //printf("%d ",(int)(kvpos->value));
  }

  //printSymboltable(table,0);
  reformStructualProduction(table);
  //printProductionWithName(table,0);
  
  buildFirst(table);
}

//return a map from src(key) to dst(value)
kvpair *copyTokentable(grammar *src,symboltable *dst)
{
  kvpair *ret=createkvpairhead();
  symbolitem *start=searchSymboltableById(src->table,src->start);
  symbolattr tmpattr={.type=S_NONTERMINAL};
  int newid=insertSymboltable(dst,start->name,&tmpattr);
  intkvpairinsert(ret,src->start,newid);
  
  production *srcprod=start->attr->attr.prod;
  productionbody *pbpos;
  prod_for_each_prodbody(pbpos,srcprod){
    pbodyunit *phead=pbpos->unit;
    pbodyunit *ppos;
    for_each_pbodyunit(ppos,phead){
      kvpair *pair=_copyTokentable(src->table,ppos,dst);
      if(kvpairishead(pair))
	intkvpairlistinsert(ret,pair);
      else
	_intkvpairinsert(ret,pair);
    }
  }
  return ret;
}

kvpair *_copyTokentable(symboltable *src,pbodyunit *unit,symboltable *dst)
{
  if(unit->type==P_NONTERMINAL){
    int id=unit->value.index;
    symbolitem *item=searchSymboltableById(src,id);
    int newid=insertSymboltable(dst,item->name,&terminalattr);
    kvpair *ret=createkvpair(id,newid);
    return ret;
  }
  if(unit->type==P_TERMINAL){
    int id=unit->value.index;
    char *tmp=(char*)malloc(2);
    tmp[0]=(char)id;
    tmp[1]='\0';
    int newid=insertSymboltable(dst,tmp,&terminalattr);
    kvpair *ret=createkvpair(id,newid);
    return ret;
  }
  if(unit->type==P_COMBINE){
    kvpair *ret=createkvpairhead();
    pbodyunit *head=unit->value.nest;
    pbodyunit *ppos;
    for_each_pbodyunit(ppos,head){
      kvpair *pair=_copyTokentable(src,ppos,dst);
      _intkvpairinsert(ret,pair);
    }
    return ret;
  }
  return NULL;
}

#endif
