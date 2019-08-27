#ifndef __KVPAIR_H
#define __KVPAIR_H

#include "list.h"

#include <stdlib.h>

typedef struct kvpair kvpair;
struct kvpair{
  void *key;
  void *value;
  struct list_head list;
};

#define kvpairappend(head,_key,_value)				\
  do{								\
    kvpair *tmp=createkvpair(_key,_value);	\
    listaddtail(&(tmp->list),&(head->list));			\
  }while(0)

#define createkvpair(key,value)			\
  _createkvpair((void*)key,(void*)value)

#define createkvpairhead()			\
  createkvpair(0,0)

#define kvpairishead(pos)			\
  (pos->key==NULL && pos->value==NULL)

#define kvpairfind(head,key,isequal)		\
  _kvpairfind(head,(void*)key,isequal)

#define for_each_kvpair(pos,head)		\
  list_for_each_entry(pos,&(head->list),list)

kvpair *_createkvpair(void *key,void *value);
void *_kvpairfind(kvpair *head,void *key,int (*isequal)(void*,void*));
kvpair *kvpaircopy(kvpair *src);

kvpair *_createkvpair(void *key,void *value)
{
  kvpair *ret=(kvpair*)malloc(sizeof(kvpair));
  ret->key=key;
  ret->value=value;
  list_init(ret->list);
  return ret;
}

void *_kvpairfind(kvpair *head,void *key,int (*isequal)(void*,void*))
{
  kvpair *pos;
  list_for_each_entry(pos,&(head->list),list){
    if(isequal(pos->key,key))
      return pos->value;
  }
  return NULL;
}

kvpair *kvpaircopy(kvpair *src)
{
  void *key=src->key,*value=src->value;
  return _createkvpair(key,value);
}

//==============compare function
//a,b is *kvpair
int intkvpaircmp(void *a,void *b);
//a is *kvpair, b is int
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

//=====================for int kvpair
void intkvpairinsert(kvpair *head,int key,int value);
void intkvpairlistinsert(kvpair *head,kvpair *listhead);
int _intkvpairinsert(kvpair *head,kvpair *new);
int intkvpairfind(kvpair *head,int key);
void printintkvpair(kvpair *head);

void intkvpairinsert(kvpair *head,int key,int value)
{
  kvpair *new=createkvpair(key,value);
  _intkvpairinsert(head,new);
}

void intkvpairlistinsert(kvpair *head,kvpair *listhead)
{
  kvpair *pos;
  for_each_kvpair(pos,listhead){
    kvpair *copy=kvpaircopy(pos);
    _intkvpairinsert(head,copy);
  }
}

int _intkvpairinsert(kvpair *head,kvpair *new)
{
  list_insert_order(head,list,key,new,intcmp);
}

int intkvpairfind(kvpair *head,int key)
{
  return (int)(kvpairfind(head,key,funcintisequal));
}


void printintkvpair(kvpair *head)
{
  kvpair *pos;
  list_for_each_entry(pos,&(head->list),list){
    printf("%d %d\n",(int)(pos->key),(int)(pos->value));
  }
}

#endif
