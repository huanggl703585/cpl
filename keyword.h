#ifndef __KEYWORD_H
#define __KEYWORD_H

#include "list.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct kwdict kwdict;
struct kwdict{
  int len;
  struct list_head list;
  char str[];
};

#define for_each_kwdict(pos,head)		\
  list_for_each_entry(pos,&(head->list),list)

#define kwdict_insert_order(listhead,new)	\
  list_insert_order(listhead,list,str,new,strcmp)

#define kwdict_find_key(pos,listhead,target)	\
  list_find_key(pos,listhead,list,str,target,strequal)

kwdict *createkwdicthead();
kwdict *createkwdictnode(char *str);
void kwdictinsert(kwdict *head,char *str);
int kwdictfind(kwdict *head,char *str);

void printkwdict(kwdict *head);

kwdict *createkwdicthead()
{
  kwdict *ret=(kwdict*)malloc(sizeof(kwdict));
  list_init(ret->list);
  return ret;
}

kwdict *createkwdictnode(char *str)
{
  int len=strlen(str);
  kwdict *ret=(kwdict*)malloc(sizeof(kwdict)+len+1);
  ret->len=len;
  list_init(ret->list);
  strcpy((ret->str),str);
  return ret;
}

void kwdictinsert(kwdict *head,char *srcstr)
{
  kwdict *new=createkwdictnode(srcstr);
  kwdict_insert_order(head,new);
}

int kwdictfind(kwdict *head,char *target)
{
  kwdict *pos;
  kwdict_find_key(pos,head,target);
  if(pos==head) return 0;
  return 1;
}

void printkwdict(kwdict *head)
{
  kwdict *pos;
  for_each_kwdict(pos,head){
    printf("%s\n",pos->str);
  }
}
#endif
