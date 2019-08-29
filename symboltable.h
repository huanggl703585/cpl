#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H

#include <stdlib.h>
#include "symbolattr.h"
#include "hash.h"
#include "symboltableoption.h"

#define ASCII_BIAS 127

typedef struct symboltable symboltable;
typedef struct symbolitem symbolitem;

struct symbolitem{
  int id; //id starts from 0
  char *name;
  int derivecnt;
  symbolattr *attr;
};

struct symboltable{
  int size;
  int count; //count is the number of symbol, not include bias
  int bias; //for idarray
  int *idarray;
  int *toposort;
  int topofirst;
  symbolitem **table;   //it stores non-terminal
  charmapper **terminal;//it stores terminal
  symboltableoption option;
};

#define symbol_table_basic_size 1024

#define iterate_symbol_attr(table,func)					\
  do{									\
    for(int __i=0;__i<table->count;__i++){				\
      symbolitem *__item=searchSymboltableById(table,__i+table->bias);	\
      if(__item->attr->type==S_TERMINAL) continue;			\
      func(__item->attr);						\
    }									\
  }while(0)

symboltable* createSymboltable(size_t size,int bias);
int insertSymboltable(symboltable* st,char *str,symbolattr *attr);
symbolitem* searchSymboltableByName(symboltable *st,char *str);
symbolitem* searchSymboltableById(symboltable *st,int id);
int searchSymbolIdByName(symboltable *st,char *str);
int changeSymboltableByName(symboltable *st,char *str,symbolattr *attr);
int changeSymboltableById(symboltable *st,int id,symbolattr *attr);

symboltableoption *symboltableGetOption(symboltable *table);
void symboltableSetOption(symboltable *table,symboltableoption *option);

symboltable *createSymboltable(size_t size,int bias)
{
  symboltable* ret=(symboltable*)malloc(sizeof(symboltable));
  ret->size=size;
  ret->count=0;
  ret->bias=bias;
  ret->idarray=(int*)malloc(sizeof(int)*(ret->size));
  ret->toposort=(int*)malloc(sizeof(int)*(ret->size));
  ret->topofirst=0;
  ret->table=(symbolitem**)malloc(sizeof(symbolitem*)*(ret->size));
  ret->terminal=(charmapper**)malloc(sizeof(charmapper*)*(ret->bias));
  bzero(ret->idarray,sizeof(int)*(ret->size));
  bzero(ret->table,sizeof(symbolitem*)*(ret->size));
  bzero(ret->terminal,sizeof(charmapper*)*(ret->bias));
  bzero(ret->toposort,sizeof(int)*(ret->size));
  init_symboltable_option(ret->option);
  return ret;
}

int insertSymboltable(symboltable *st,char *str,symbolattr *attr)
{
  unsigned int hashvalue = strHash(str)%(st->size);
  unsigned int head=hashvalue;
  symbolitem *item=(st->table)[hashvalue];

  while(item!=NULL){
    if(strcmp(item->name,str)==0){
      //whether dupcate name is an error depends on table->option
      if(st->option.duplicate==1)
	return item->id;
      else
	return -1;
    }
    hashvalue=(hashvalue+1)%(st->size);
    item=(st->table)[hashvalue];
    if(hashvalue==head) // the table is full
      return 1;
  }
  
  item=(symbolitem*)malloc(sizeof(symbolitem));
  int len=strlen(str);
  char *pt=(char*)malloc(len+1);
  memcpy(pt,str,len+1);
  item->name=pt;
  item->id=((st->count)+st->bias);
  item->derivecnt=0;
  if(attr!=NULL){
    item->attr=(symbolattr*)malloc(sizeof(symbolattr));
    memcpy(item->attr,attr,sizeof(symbolattr));
  }
  else
    item->attr=createSymbolAttr(item->id);
  st->table[hashvalue]=item;
  st->idarray[item->id]=hashvalue;
  st->count++;
  return item->id;
}

symbolitem* searchSymboltableByName(symboltable *st,char *str)
{
  unsigned int hashvalue = strHash(str)%(st->size);
  unsigned int head=hashvalue;
  symbolitem *item=(st->table)[hashvalue];
  
  do{
    if(strcmp(str,item->name)==0)
      return item;
    item=(st->table)[++hashvalue];
  }while(item!=NULL && hashvalue!=head);
  
  return NULL;
}

int searchSymbolIdByName(symboltable *st,char *str)
{
  symbolitem *item=searchSymboltableByName(st,str);
  return item->id;
}

symbolitem* searchSymboltableById(symboltable *st,int id)
{
  int pos=st->idarray[id];
  symbolitem* item=st->table[pos];
  return item;
}

int changeSymboltable(symboltable *st,char *str,symbolattr *attr)
{
  unsigned int hashvalue = strHash(str)%(st->size);
  unsigned int head=hashvalue;
  symbolitem *item=(st->table)[hashvalue];
  
  do{
    if(strcmp(str,item->name)==0){
      item->attr=attr;
      return 0;
    }
    item=(st->table)[++hashvalue];
  }while(item!=NULL && hashvalue!=head);
  
  return 1;
}

int changeSymboltableById(symboltable *st,int id,symbolattr *attr)
{
  int pos=st->idarray[id];
  symbolitem *item=st->table[pos];
  item->attr=attr;
}

symboltableoption *symboltableGetOption(symboltable *table)
{
  symboltableoption *ret=(symboltableoption*)malloc(sizeof(symboltableoption));
  memcpy(ret,&(table->option),sizeof(symboltableoption));
  return ret;
}

void symboltableSetOption(symboltable *table,symboltableoption *option)
{
  memcpy(&(table->option),option,sizeof(symboltableoption));
}

//=========================================
#define isterminal(table,index)			\
  (index<table->bias || isToken(table,index))
int isToken(symboltable *table,int id);
void appendProdbodyByName(symboltable *table,productionbody *pbpos,char *str);

int isToken(symboltable *table,int id)
{
  symbolitem *item=searchSymboltableById(table,id);
  return item->attr->type==S_TERMINAL;
}

void appendProdbodyByName(symboltable *table,productionbody *pbpos,char *str)
{
  int id=searchSymbolIdByName(table,str);
  appendProdbody(pbpos,id);
}

//==========================================
#define symbolDeriveName(newname,oriname,ulen)			\
  char *newname=(char*)malloc(strlen(oriname)+1+ulen);		\
  do{								\
    char padding[ulen+1];					\
    for(int i=0;i<ulen;i++) padding[i]='_';			\
    padding[ulen]='\0';						\
    sprintf(newname,"%s%s",padding,oriname);			\
  }while(0)

int deriveNewSymbol(symboltable *table,symbolitem *origin);

int deriveNewSymbol(symboltable *table,symbolitem *origin)
{
  //symbolattr *attr=createsymbolattr();
  origin->derivecnt++;
  symbolDeriveName(newname,origin->name,origin->derivecnt);
  return insertSymboltable(table,newname,NULL);
}

//===========================================
void reformLexcialProduction(symboltable *table);
void reformStructualProduction(symboltable *table);

void prodInUnit(symboltable *table);
void reviseUnitType(symboltable *table);
void _reviseUnitType(symboltable *table,pbodyunit *unit);

void disassembleOr(symboltable *table);
void elimateParenthese(symboltable *table);
void elimateOr(symboltable *table);
void extractLeftLcpOne(symboltable *table,symbolitem *item);
void extractLeftLcp(symboltable *table);
void elimateLeftRecursionOne(symboltable *table,symbolitem *item);
void elimateLeftRecursion(symboltable *table);
void printTablePunit(symboltable *table);

void pbodyunitAddEdge(pbodyunit *list,node nodearr[],int *nodenum,int head,int bias);
void symbolToposort(symboltable *table);

//solve left recursion before
void symbolSetType(symboltable *table);
void printSymbolType(symboltable *table);

void symbolSetMapper(symboltable *table);
void _symbolSetMapper(symboltable *table,production *prod);
void printSymbolMapper(symboltable *table);

void symbolElimateNest(symboltable *table);

//option==0 print in id order
//option==1 print in toposort order
void printProductionWithName(symboltable *table,int option);
void printSymboltable(symboltable *table,int option);


void reformLexcialProduction(symboltable *table)
{
  prodInUnit(table);
  //printTablePunit(table);
  elimateParenthese(table);
  elimateOr(table);
  extractLeftLcp(table);
  elimateLeftRecursion(table);
  symbolSetType(table);
  //printsymboltype(table);
  symbolSetMapper(table);
  //printsymbolmapper(table);
  symbolToposort(table);
  //printproductionwithname(table,1);
}

void reformStructualProduction(symboltable *table)
{
  prodInUnit(table);
  reviseUnitType(table);
  //printTablePunit(table);
  elimateParenthese(table);
  elimateOr(table);
  extractLeftLcp(table);
  elimateLeftRecursion(table);
  symbolSetType(table);
  //printSymbolType(table);
  //symbolToposort(table);
  //printSymboltable(table,0);
  //printProductionWithName(table,0);
  //TODO : UNTEST
  symbolElimateNest(table);
}

void prodInUnit(symboltable *table)
{
  iterate_symbol_attr(table,_prodInUnit);
}

void reviseUnitType(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type==S_TERMINAL) continue;
    production *prod=item->attr->attr.prod;
    productionbody *pbpos;
    prod_for_each_prodbody(pbpos,prod){
      pbodyunit *listhead=pbpos->unit;
      pbodyunit *pos;
      for_each_pbodyunit(pos,listhead){
	_reviseUnitType(table,pos);
      }
    }
  }
}

void _reviseUnitType(symboltable *table,pbodyunit *unit)
{
  if(unit->type==P_NONTERMINAL){
    int index=unit->value.index;
    symbolitem *item=searchSymboltableById(table,index);
    if(item->attr->type==S_TERMINAL)
      unit->type=P_TERMINAL;
  }
  else if(unit->type==P_COMBINE){
    pbodyunit *listhead=unit->value.nest;
    pbodyunit *pos;
    for_each_pbodyunit(pos,listhead){
      _reviseUnitType(table,pos);
    }
  }
}

void printTablePunit(symboltable *table)
{
  //iterate_symbol_attr(table,_printTablePunit);  
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    printf("%s %d\n",item->name,item->id);
    if(item->attr->type==S_TERMINAL) continue;
    _printTableUnit(item->attr);
  }
}

void elimateParenthese(symboltable *table)
{
  iterate_symbol_attr(table,_elimateParenthese);
}

void elimateOr(symboltable *table)
{
  iterate_symbol_attr(table,_elimateOr);
}

void extractLeftLcpOne(symboltable *table,symbolitem *item)
{
  production *prod=item->attr->attr.prod;
  int pt;
  //the loop continue until there no other common prefix
  while(1){
    int size=prod->cnt;
    int mark[size];
    pbodyunit* unitarr[size];
    productionbody *pbpos;
    pt=0;
    prod_for_each_prodbody(pbpos,prod)
      unitarr[pt++]=pbpos->unit;
    pbodyunit *lcphead;
    int headindex;

    if(pbodyunitFindLcp(unitarr,mark,size,&lcphead,&headindex)==0)
      return ;
  
    int newid=deriveNewSymbol(table,item);
    symbolitem *newitem=searchSymboltableById(table,newid);
    production *newprod=newitem->attr->attr.prod;
    pbodyunit *newunit=createPbodyunit();
    newunit->type=P_NONTERMINAL;newunit->value.index=newid;
    pbodyunit *lcp=pbodyunitCopy(lcphead,unitarr[headindex]);
    pbodyunit *newbody=pbodyunitCopy(lcphead,unitarr[headindex]);
    pbodyunitAppend(newunit,newbody);
    //printpbodyunit(lcp);
    //A-> alpha A` | beta
    //add new to A
    productionbody *originnewbody=createProdbodyLinkProd(prod);
    originnewbody->unit=newbody;
    originnewbody->cnt=headindex+2;
    //prodbodyappendPbodyunit(originnewbody,newunit);
    //delete A's prod, add prod to A`
    pbpos=prod->productionbody;
    productionbody *pbtmp=prodbodyNext(pbpos);
    for(pt=0;pt<size;pt++){
      pbpos=pbtmp;
      pbtmp=prodbodyNext(pbtmp);
      if(mark[pt]!=0){
	if(!pbodyunitListIsEmpty(unitarr[pt])){
	  productionbody *tmppb=createProdbodyLinkProd(newprod);
	  prodbodyAppendPbodyunitList(tmppb,unitarr[pt]);
	}
	else
	  prodAppendEmptyProdbody(newprod);
	productionDrop(prod,pbpos);
      }
    }
  }
}
/*
      A  -> ab1 | ab2 | ... | abn | gamma
  ==> A  -> aA` | gamma
      A` -> b1 | b2 | ... | bn
*/
void extractLeftLcp(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type==S_TERMINAL) continue;
    extractLeftLcpOne(table,item);
  }
}

void elimateLeftRecursionOne(symboltable *table,symbolitem *item)
{
  production *prod=item->attr->attr.prod;
  int head=prod->head;
  int pcnt=prod->cnt;
  int mark[pcnt];
  pbodyunit *unitarr[pcnt];
  productionbody *pbpos;
  int pt=0;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *tmp=pbpos->unit;
    unitarr[pt++]=tmp;
  }

  if(!(pbodyunitFindLeftRecursion(unitarr,mark,pcnt,head)))
    return ;

  int newid=deriveNewSymbol(table,item);
  symbolitem *newitem=searchSymboltableById(table,newid);
  production *newprod=newitem->attr->attr.prod;
  pbodyunit *newunit=createPbodyunit();
  newunit->type=P_NONTERMINAL;newunit->value.index=newid;
  pt=0;
  prod_for_each_prodbody(pbpos,prod){
    if(mark[pt]==0)
      prodbodyAppendPbodyunit(pbpos,newunit);
    else{
      productionbody *tmppb=createProdbodyLinkProd(newprod);
      pbodyunit *liststart=pbodyunitNext(unitarr[pt]);
      prodbodyAppendPbodyunitList(tmppb,liststart);
      prodbodyAppendPbodyunit(tmppb,newunit);
      
      productionDrop(prod,pbpos);
    }
    pt++;
  }
  prodAppendEmptyProdbody(newprod);
}

/*
       A  -> A alpha
  ==>  A  -> beta A`
       A` -> alpha A` | empty
*/
void elimateLeftRecursion(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type==S_TERMINAL) continue;
    elimateLeftRecursionOne(table,item);
  }
}

//============================type,mapper

#define haveMapper(table,id)			\
  (table->terminal[id]!=NULL)

void symbolSetType(symboltable *table)
{
  iterate_symbol_attr(table,_symbolSetType);
}

void printSymbolType(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    printf("%s\t",item->name);
    _printSymbolType(item->attr);
  }
}

void symbolSetMapper(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type==S_TERMINALSET){
      production *prod=item->attr->attr.prod;
      _symbolSetMapper(table,prod);
    }
  }
}

void _symbolSetMapper(symboltable *table,production *prod)
{
  int mapto=prod->head;
  productionbody *pbpos;
  symbolitem *item;
  charmapper *mapper=NULL;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *u=pbpos->unit;
    u=pbodyunitNext(u);
    int value=u->value.index;
    if(table->terminal[value]==NULL)
      table->terminal[value]=createCharmapper(value);
    charmapperAppend(table->terminal[value],mapto);
  }
}

void printSymbolMapper(symboltable *table)
{
  for(int i=0;i<table->bias;i++){
    charmapper *mapper=table->terminal[i];
    if(mapper!=NULL)
      printCharmapper(table->terminal[i]);
  }
}

//===============================toposort

void pbodyunitAddEdge(pbodyunit *list,node nodearr[],int *nodenum,int head,int bias)
{
  pbodyunit *pos;
  for_each_pbodyunit(pos,list){
    if(pos->type==P_NONTERMINAL){
      int key=pos->value.index;
      if(key!=head){
	int num=*nodenum;
	arraynodeaddedge(nodearr,key-bias,head-bias,num);
	*nodenum=num;
      }
    }
    else if(pos->type==P_COMBINE){
      pbodyunit *nest=pos->value.nest;
      pbodyunitAddEdge(nest,nodearr,nodenum,head,bias);
    }
  }
}
void symbolToposort(symboltable *table)
{
  //mind the difference of domain of table index and node index
  int nodenum=table->count;
  //size of nodearr is sum of node and edge
  node nodearr[1024];
  for(int i=0;i<nodenum;i++){
    initnode(nodearr[i],i);
  }
  
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type!=S_NONTERMINAL) continue;

    production *prod=item->attr->attr.prod;
    int head=prod->head;
    productionbody *pbpos;
    prod_for_each_prodbody(pbpos,prod){
      pbodyunit *unitlist=pbpos->unit;
      pbodyunitAddEdge(unitlist,nodearr,&nodenum,head,table->bias);
    }
  }
  arraytopologysort(nodearr,table->count,table->toposort);
  for(int i=0;i<table->count;i++)
    table->toposort[i]+=table->bias;
}

//========================re_exp

//return a expanded retree
re_node* symboltableBuildRetree(symboltable *table,int start);
//combine until start symbol, because we need to mark other symbol
void symboltableRetreeCombine(symboltable *table,int start);
void doSymboltableRetreeCombine(symboltable *table,re_node *root);
void symboltableRetreeCopyCombine(symboltable *table,re_node *root);
void symboltableRetreeSetMark(symboltable *table,int start);
void symboltablePrintRetree(symboltable *table);

re_node* symboltableBuildRetree(symboltable *table,int start)
{
  iterate_symbol_attr(table,_symboltableBuildRetree);
  symboltableRetreeCombine(table,start);
  symboltableRetreeSetMark(table,start);
  
  symbolitem *startitem=searchSymboltableById(table,start);
  re_node *starttree=startitem->attr->attr.prod->retree;
  symboltableRetreeCopyCombine(table,starttree);
  re_node *ret=retreeexpand(starttree,0);
  retreecount(ret);
  return ret;
}

//combine every symbol's retree except start symbol
//for example, if production is token ::= identifier | equivalence
//then the retree of identifier and equivalence is combined by reference
//the retree of start symbol should be combined by copy
void symboltableRetreeCombine(symboltable *table,int start)
{
  for(int i=0;i<table->count;i++){
    if(table->toposort[i]==start) continue;
    symbolitem *item=searchSymboltableById(table,table->toposort[i]);
    if(item->attr->type==S_TERMINALSET) continue;

    doSymboltableRetreeCombine(table,item->attr->attr.prod->retree);
  }
}

void doSymboltableRetreeCombine(symboltable *table,re_node *root)
{
  re_node *left=root->left,*right=root->right;
  if(left!=NULL && left->type==RE_OPERAND && left->value>table->bias){
    symbolitem *newitem=searchSymboltableById(table,left->value);
    re_node *newtree=newitem->attr->attr.prod->retree;
    retreereplace(root,isleft,newtree);
  }
  if(right !=NULL && right->type==RE_OPERAND && right->value>table->bias){
    symbolitem *newitem=searchSymboltableById(table,right->value);
    re_node *newtree=newitem->attr->attr.prod->retree;
    retreereplace(root,isright,newtree);
  }
  if(left!=NULL && left->type==RE_OPERATOR)
    doSymboltableRetreeCombine(table,left);
  if(right!=NULL && right->type==RE_OPERATOR)
    doSymboltableRetreeCombine(table,right);
}

//special for start symbol
void symboltableRetreeCopyCombine(symboltable *table,re_node *root)
{
  re_node *left=root->left,*right=root->right;
  re_node *newtree;
  if(left!=NULL && left->type==RE_OPERAND){
    if(left->value>table->bias){
      symbolitem *newitem=searchSymboltableById(table,left->value);
      newtree=retreecopy(newitem->attr->attr.prod->retree);
    }
    else
      newtree=renodecopy(left);
    newtree=retreeexpand(newtree,newtree->mark);
    retreereplace(root,isleft,newtree);
  }
  if(right !=NULL && right->type==RE_OPERAND){
    if(right->value>table->bias){
      symbolitem *newitem=searchSymboltableById(table,right->value);
      newtree=retreecopy(newitem->attr->attr.prod->retree);
    }
    else
      newtree=renodecopy(right);
    newtree=retreeexpand(newtree,newtree->mark);
    retreereplace(root,isright,newtree);
  }
  if(left!=NULL && left->type==RE_OPERATOR)
    symboltableRetreeCopyCombine(table,left);
  if(right!=NULL && right->type==RE_OPERATOR)
    symboltableRetreeCopyCombine(table,right);
}

void symboltableRetreeSetMark(symboltable *table,int start)
{
  symbolitem *item=searchSymboltableById(table,start);
  re_node *root=item->attr->attr.prod->retree;
  re_node *stack[1024];
  int pt=0;
  stack[pt++]=root;
  while(--pt>=0){
    re_node *curnode=stack[pt];
    if(curnode->type==RE_OPERAND){
      if(curnode->value>table->bias){
	symbolitem *curitem=searchSymboltableById(table,curnode->value);
	retreesetmark(curitem->attr->attr.prod->retree,curnode->value);
      }
      else
	curnode->mark=curnode->value;
    }
    if(curnode->left!=NULL) stack[pt++]=curnode->left;
    if(curnode->right!=NULL) stack[pt++]=curnode->right;
  }
}

void symboltablePrintRetree(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type==S_TERMINALSET) continue;

    printretree(item->attr->attr.prod->retree);
  }
}

//========================
//used for structual grammar
void symbolElimateNest(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type!=S_NONTERMINAL) continue;

    production *prod=item->attr->attr.prod;
    productionbody *pbpos;
    prod_for_each_prodbody(pbpos,prod){
      pbodyunit *list=pbpos->unit;
      elimatePbodyunitNest(list);
    }
  }
}

//========================print/test

void printProductionWithName(symboltable *table,int option)
{
  for(int i=0;i<table->count;i++){
    int id;
    if(option==0)
      id=i+table->bias;
    else
      id=table->toposort[i];
    symbolitem *item=searchSymboltableById(table,id);
    symbolattr *attr=item->attr;
    if(attr->type==S_TERMINAL){
      printf("%s\n",item->name);
      continue;
    }
    production *prod=attr->attr.prod;
    int head=prod->head;
    item=searchSymboltableById(table,head);
    printf("head %s cnt %d\n",item->name,prod->cnt);
    productionbody *pbpos;
    prod_for_each_prodbody(pbpos,prod){
      printf("cnt %d\n",pbpos->cnt);
      printPbodyunit(pbpos->unit);
    }
    printf("\n");
  }
}

void printSymboltable(symboltable *table,int option)
{
  for(int i=0;i<table->count;i++){
    int id;
    if(option==0)
      id=i+table->bias;
    else
      id=table->toposort[i];
    symbolitem *item=searchSymboltableById(table,id);
    printf("%d %s\n",item->id,item->name);
  }
}
#endif
