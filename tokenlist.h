#ifndef __TOKENLIST_H
#define __TOKENLIST_H

#include "symbolpos.h"

typedef struct token token;
struct token{
  int gindex;
  int sindex;
  struct list_head list;
};

typedef struct tokenlist tokenlist;
struct tokenlist{
  symboltable *gtable; //in grammar, like identifier\constant and so on
  symboltable *stable; //in symbol, like i,tep,323
  token *tlist;
};

#define tokenNext(tokenpt)			\
  list_next_entry(tokenpt,list)

#define tokenlistFirst(tlisthead)		\
  list_first_entry(&(tlisthead->tlist->list),token,list)

#define for_each_token(pos,listhead)		\
  list_for_each_entry(pos,&(listhead->tlist->list),list)

#define appendTokenlist(tokenlist,sindex,gindex) ({		\
    token *__tmp=createToken(gindex,sindex);			\
    listaddtail(&(__tmp->list),&(tokenlist->tlist->list));	\
    __tmp;})

token *createToken(int gindex,int sindex);
tokenlist *createTokenlist(symboltable *gtable,symboltable *stable);
token* appendToken(tokenlist *tlist,char *str,int gindex);

token *createToken(int gindex,int sindex)
{
  token *ret=(token*)malloc(sizeof(token));
  ret->gindex=gindex;
  ret->sindex=sindex;
  list_init(ret->list);
  return ret;
}

tokenlist *createTokenlist(symboltable *gtable,symboltable *stable)
{
  tokenlist *ret=(tokenlist*)malloc(sizeof(tokenlist));
  ret->gtable=gtable;
  ret->stable=stable;
  ret->tlist=createToken(0,0);
  return ret;
}

token* appendToken(tokenlist *tlist,char *str,int gindex)
{
  int sid=insertSymboltable(tlist->stable,str,NULL);
  token *ret=appendTokenlist(tlist,sid,gindex);
  //printf("%d %d\n",sid,gindex);
  return ret;
}

//=============================test
void printtokenlist(tokenlist *tlist)
{
  token *head=tlist->tlist;
  token *pos;
  list_for_each_entry(pos,&(head->list),list){
    printf("%d %d\n",pos->gindex,pos->sindex);
  }
}

#endif
