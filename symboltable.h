#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H

#include <stdlib.h>
#include "symbolattr.h"
#include "hash.h"

typedef struct symboltable symboltable;
typedef struct symbolitem symbolitem;

struct symbolitem{
  int id; //id starts from 0
  char *name;
  symbolattr *attr;
};

struct symboltable{
  int size;
  int count;
  symbolitem **table;
};

#define symbol_table_basic_size 1024

symboltable* createsymboltable(size_t size);
int insertsymboltable(symboltable* st,char *str,symbolattr *attr);
symbolattr* searchsymboltablebyname(symboltable *st,char *str);
int changesymboltablebyname(symboltable *st,char *str,symbolattr *attr);


symboltable *createsymboltable(size_t size)
{
  symboltable* ret=(symboltable*)malloc(sizeof(symboltable));
  ret->size=size;
  ret->count=0;
  ret->table=(symbolitem**)malloc(sizeof(symbolitem*)*(ret->size));
  bzero(ret->table,sizeof(symbolitem*)*(ret->size));
  return ret;
}

int insertsymboltable(symboltable *st,char *str,symbolattr *attr)
{
  unsigned int hashvalue = strhash(str)%(st->size);
  unsigned int head=hashvalue;
  symbolitem *item=(st->table)[hashvalue];

  while(item!=NULL){
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
  item->id=((st->count)++);
  item->attr=attr;
  st->table[hashvalue]=item;
  return 0;
}

symbolattr* searchsymboltable(symboltable *st,char *str)
{
  unsigned int hashvalue = strhash(str)%(st->size);
  unsigned int head=hashvalue;
  symbolitem *item=(st->table)[hashvalue];
  
  do{
    if(strcmp(str,item->name)==0)
      return item->attr;
    item=(st->table)[++hashvalue];
  }while(item!=NULL && hashvalue!=head);
  
  return NULL;
}

int changesymboltable(symboltable *st,char *str,symbolattr *attr)
{
  unsigned int hashvalue = strhash(str)%(st->size);
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
#endif
