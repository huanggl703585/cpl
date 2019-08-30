#ifndef __BUILDFIRST_H
#define __BUILDFIRST_H

#include "sliststack.h"
#include "symboltable.h"
#include "tokenlist.h"
#include "ast.h"

#include <limits.h>

#define EMPTY_SYMBOL INT_MAX

#define setHaveEmpty(setsrc)			\
  ((findset(setsrc,EMPTY_SYMBOL))!=0)

//=======================build first set
//set's element is symboltable's index
void buildFirst(symboltable *table);
void buildFirstTerminal(symboltable *table);
//return 1 if the firstset has added new
int buildFirstOne(symboltable *table,int id);
int addFirstSet(symboltable *table,symbolitem *item,pbodyunit *list);
pbodyunit *findFirstSymbol(symboltable *table,pbodyunit *list);
//0 stands for empty
int FirstsetHaveEmpty(symboltable *table,int id);

void printFirstSet(symboltable *table);

void buildFirst(symboltable *table)
{
  buildFirstTerminal(table);
  int flag=1;
  while(flag){
    flag=0;
    for(int i=0;i<table->count;i++){
      int tmp=buildFirstOne(table,i+table->bias);
      if(tmp!=0) flag=1;
    }
  }
}

void buildFirstTerminal(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type!=S_TERMINAL) continue;

    set *target=item->attr->first;
    insertset(target,i+table->bias);
    item->attr->first=target;
    //printset(target);
  }
}

int buildFirstOne(symboltable *table,int id)
{
  symbolitem *item=searchSymboltableById(table,id);
  if(item->attr->type==S_TERMINAL) return 0;
  
  production *prod=item->attr->attr.prod;
  productionbody *pbpos;
  int ret=0;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *ulist=pbpos->unit;
    int tmp=addFirstSet(table,item,ulist);
    if(tmp!=0) ret=1;
  }
  return ret;
}

int addFirstSet(symboltable *table,symbolitem *item,pbodyunit *list)
{
  pbodyunit *firstsymbol; //the first symbol that FIRST() has no empty
  firstsymbol=findFirstSymbol(table,list);
  if(firstsymbol==NULL) firstsymbol=list;
  else firstsymbol=pbodyunitNext(firstsymbol);

  int ret=0,tmp;
  //symbol between (list, firstsymbol) is aviliable
  for(pbodyunit *pos=pbodyunitNext(list);pos!=firstsymbol;pos=pbodyunitNext(pos)){
    if(pos->type==P_NONTERMINAL){
      int index=pos->value.index;
      symbolitem *srcitem=searchSymboltableById(table,index);
      if((intersectset(&(item->attr->first),(srcitem->attr->first)))!=0)
	ret=1;
    }
    else if(pos->type==P_TERMINAL){
      int index=pos->value.index;
      if((insertset((item->attr->first),index))!=0)
	ret=1;
    }
    else if(pos->type==P_EMPTY){
      if((insertset((item->attr->first),EMPTY_SYMBOL))!=0)
	ret=1;
    }
  }
  //printset(item->attr->first);
  return ret;
}

//find the first symbol that FIRST() has no empty
pbodyunit *findFirstSymbol(symboltable *table,pbodyunit *list)
{
  pbodyunit *pos;
  for_each_pbodyunit(pos,list){
    if(pos->type==P_NONTERMINAL){
      int index=pos->value.index;
      symbolitem *item=searchSymboltableById(table,index);
      set *first=item->attr->first;
      if(!setHaveEmpty(first)) return pos;
    }
    else if(pos->type==P_TERMINAL){
      return pos;
    }
  }
  return NULL;
}

void printFirstSet(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    set *target=item->attr->first;
    printset(target);
  }
  printf("\n");
}


//=============================build follow set

#define END_SYMBOL INT_MAX-1

void buildFollow(symboltable *table);
int addFollowSet(symboltable *table,symbolitem *item,pbodyunit *plist);
set *findPbodyunitListFirst(symboltable *table,pbodyunit *plist);
void printFollowSet(symboltable *table);

void buildFollow(symboltable *table)
{
  symbolitem *startitem=searchSymboltableById(table,table->start);
  insertset((startitem->attr->follow),END_SYMBOL);

  int flag=1;
  while(flag){
    flag=0;
    for(int i=0;i<table->count;i++){
      symbolitem *item=searchSymboltableById(table,i+table->bias);
      if(item->attr->type==S_TERMINAL) continue;
      production *prod=item->attr->attr.prod;
      productionbody *pbpos;
      prod_for_each_prodbody(pbpos,prod){
	if((addFollowSet(table,item,pbpos->unit))!=0)
	  flag=1;
      }
    }
    //printFollowSet(table);
  }
}

//book p141 
int addFollowSet(symboltable *table,symbolitem *item,pbodyunit *plist)
{
  int ret=0;
  pbodyunit *pos;
  for_each_pbodyunit(pos,plist){
    if(pos->type==P_NONTERMINAL){
      set *src=findPbodyunitListFirst(table,pos);
      //avltreeprint(src);
      symbolitem *dstitem=searchSymboltableById(table,pos->value.index);
      if(src==NULL || findset(src,END_SYMBOL)){
	if(intersectset(&(dstitem->attr->follow),item->attr->follow))
	  ret=1;
      }
      if(intersectset(&(dstitem->attr->follow),src)!=0)
	ret=1;
    }
  }
  return ret;
}

//set's element is firstset's element (type: int)
set *findPbodyunitListFirst(symboltable *table,pbodyunit *plist)
{
  set *ret=NULL;
  int flag=1;
  pbodyunit *start=pbodyunitNext(plist);
  if(start->type==0) return NULL;
  
  while(flag){
    flag=0;

    pbodyunit *pos=start;
    while(pos->type!=P_NONTERMINAL && pos->type!=P_TERMINAL)
      pos=pbodyunitNext(pos);
    if(pos->type==P_NONTERMINAL){
      int index=pos->value.index;
      symbolitem *item=searchSymboltableById(table,index);
      intersectset(&ret,(item->attr->first));
    }
    else if(pos->type==P_TERMINAL){
      insertset(ret,(pos->value.index));
    }
    start=pbodyunitNext(pos);

    pbodyunit *next=start;
    if(next->type==P_OR) flag=1;
  }
  return ret;
}

void printFollowSet(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchSymboltableById(table,i+table->bias);
    if(item->attr->type!=S_TERMINAL){
      printf("%s\t",item->name);
      printset((item->attr->follow));
    }
  }
  printf("\n");
}

//===========================forecast table
#include "forecasttable.h"
/*
//=========================parsing

ast* LL1parsing(tokenlist *tlist);

ast* LL1parsing(tokenlist *tlist)
{
  symboltable *table=tlist->gtable;
  //TODO: table->bias should be replaced as start symbol
  int startsymbol=table->bias;
  
  sliststack *stack=createsliststack();
  initslist(tmpstart,startsymbol);
  sliststackpush(stack,tmpstart);
  
  ast *ast=createast(table);
  initast(ast,startsymbol);

  astnode *astpt=ast->root;
  token *tokenpt=tokenlistfirst(tlist);
  int stacktop;
  while((stacktop=(int)getsliststacktop(stack))!=(int)NULL){
    if(stacktop==tokenpt->gindex){
      sliststackpop(stack);
      tokenpt=tokennext(tokenpt);
    }
    else if(tokenpt->sindex < table->bias){
      return NULL;
    }
    else{
      pbody *forecast=findforecasttable(table,stacktop,tokenpt->gindex);
      if(forecast==NULL) return NULL;
      //expandast(astpt,forecast);
      sliststackpop(stack);
      sliststackpush(stack,forecast);
    }
  }
  return ast;
}
*/
#endif
