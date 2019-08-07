#ifndef __KVPAIR_H
#define __KVPAIR_H

#include "list.h"

typedef struct kvpair kvpair;
struct kvpair{
  void *key;
  void *value;
  struct list_head list;
};

#define kvpairappend(head,_key,_value)				\
  do{								\
    kvpair *tmp=_createkvpair((void*)_key,(void*)_value);	\
    listaddtail(&(tmp->list),&(head->list));			\
  }while(0)

kvpair *_createkvpair(void *key,void *value);
void printkvpair(kvpair *head);
void *kvpairfind(kvpair *head,void *key);

kvpair *_createkvpair(void *key,void *value)
{
  kvpair *ret=(kvpair*)malloc(sizeof(kvpair));
  ret->key=key;
  ret->value=value;
  list_init(ret->list);
  return ret;
}

void printkvpair(kvpair *head)
{
  kvpair *pos;
  list_for_each_entry(pos,&(head->list),list){
    printf("%d %d\n",(int)(pos->key),(int)(pos->value));
  }
}

void *kvpairfind(kvpair *head,void *key)
{
  kvpair *pos;
  list_for_each_entry(pos,&(head->list),list){
    if((int)(pos->key)==(int)(key))
      return pos->value;
  }
  return NULL;
}

//==============compare function
int intkvpaircmp(void *a,void *b);
int intkvpairkeycmp(void *a,void *b);

int intkvpaircmp(void *a,void *b)
{
  kvpair *_a=(kvpair*)a;
  kvpair *_b=(kvpair*)b;
  return intcmp((_a->key),(_b->key));
}

int intkvpairkeycmp(void *a,void *b)
{
  kvpair *_a=(kvpair*)a;
  int _b=(int)(b);
  int _keya=(int)(_a->key);
  //printf("%d %d\n",_keya,_b);
  return intcmp((_keya),(_b));
}

#endif
