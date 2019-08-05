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

#define appendtokenlist(tokenlist,gindex,sindex) ({		\
    token *__tmp=createtoken(gindex,sindex);			\
    listaddtail(&(__tmp->list),&(tokenlist->tlist->list));	\
    __tmp;})

token *createtoken(int gindex,int sindex);
tokenlist *createtokenlist(symboltable *gtable,symboltable *stable);
token* appendtoken(tokenlist *tlist,char *str,int gindex);

token *createtoken(int gindex,int sindex)
{
  token *ret=(token*)malloc(sizeof(token));
  ret->gindex=gindex;
  ret->sindex=sindex;
  list_init(ret->list);
  return ret;
}

tokenlist *createtokenlist(symboltable *gtable,symboltable *stable)
{
  tokenlist *ret=(tokenlist*)malloc(sizeof(tokenlist));
  ret->gtable=gtable;
  ret->stable=stable;
  ret->tlist=createtoken(0,0);
  return ret;
}

token* appendtoken(tokenlist *tlist,char *str,int gindex)
{
  int sid=insertsymboltable(tlist->stable,str,NULL);
  token *ret=appendtokenlist(tlist,sid,gindex);
  //printf("%d %s\n",sid,str);
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
