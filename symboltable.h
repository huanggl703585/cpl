#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H

#include <stdlib.h>
#include "symbolattr.h"
#include "hash.h"

#define ASCII_BIAS 127

typedef struct symboltable symboltable;
typedef struct symbolitem symbolitem;

struct symbolitem{
  int id; //id starts from 0
  char *name;
  symbolattr *attr;
};

struct symboltable{
  int size;
  int count; //count is the number of symbol, not include bias
  int bias; //for idarray
  int *idarray;
  int *toposort;
  symbolitem **table;
};

#define symbol_table_basic_size 1024

symboltable* createsymboltable(size_t size,int bias);
int insertsymboltable(symboltable* st,char *str,symbolattr *attr);
symbolitem* searchsymboltablebyname(symboltable *st,char *str);
symbolitem* searchsymboltablebyid(symboltable *st,int id);
int changesymboltablebyname(symboltable *st,char *str,symbolattr *attr);
int changesymboltablebyid(symboltable *st,int id,symbolattr *attr);

symboltable *createsymboltable(size_t size,int bias)
{
  symboltable* ret=(symboltable*)malloc(sizeof(symboltable));
  ret->size=size;
  ret->count=0;
  ret->bias=bias;
  ret->idarray=(int*)malloc(sizeof(int)*(ret->size));
  ret->toposort=(int*)malloc(sizeof(int)*(ret->size));
  ret->table=(symbolitem**)malloc(sizeof(symbolitem*)*(ret->size));
  bzero(ret->idarray,sizeof(int)*(ret->size));
  bzero(ret->table,sizeof(symbolitem*)*(ret->size));
  bzero(ret->toposort,sizeof(int)*(ret->size));
  return ret;
}

int insertsymboltable(symboltable *st,char *str,symbolattr *attr)
{
  unsigned int hashvalue = strhash(str)%(st->size);
  unsigned int head=hashvalue;
  symbolitem *item=(st->table)[hashvalue];

  while(item!=NULL){
    if(strcmp(item->name,str)==0)
      return -1; //we consider dupcate name is an error
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
  if(attr!=NULL)
    item->attr=attr;
  else
    item->attr=createsymbolattr(item->id);
  st->table[hashvalue]=item;
  st->idarray[item->id]=hashvalue;
  st->count++;
  return item->id;
}

symbolitem* searchsymboltablebyname(symboltable *st,char *str)
{
  unsigned int hashvalue = strhash(str)%(st->size);
  unsigned int head=hashvalue;
  symbolitem *item=(st->table)[hashvalue];
  
  do{
    if(strcmp(str,item->name)==0)
      return item;
    item=(st->table)[++hashvalue];
  }while(item!=NULL && hashvalue!=head);
  
  return NULL;
}

symbolitem* searchsymboltablebyid(symboltable *st,int id)
{
  int pos=st->idarray[id];
  symbolitem* item=st->table[pos];
  return item;
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

int changesymboltablebyid(symboltable *st,int id,symbolattr *attr)
{
  int pos=st->idarray[id];
  symbolitem *item=st->table[pos];
  item->attr=attr;
}

//=============================================
int symboladdedge(symboltable *table,int from,int to);

int symboladdedge(symboltable *table,int from,int to)
{
  symbolitem *fi=searchsymboltablebyid(table,from);
  node *fn=fi->attr->node;
  symbolitem *ti=searchsymboltablebyid(table,to);
  node *tn=ti->attr->node;
  nodeaddedge(fn,tn);
  return 0;
}

int symboltsetattr(symboltable *table);

void symboltoposort(symboltable *table);

void symboltoposort(symboltable *table)
{
  //mind the difference of domain of table index and node index
  int nodenum=table->count;
  //size of nodearr is sum of node and edge
  node nodearr[1024];
  for(int i=0;i<nodenum;i++){
    initnode(nodearr[i],i);
  }
  
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    symbolattr *attr=item->attr;
    production *prod=attr->attr.prod;
    int head=prod->head;
    productionbody *pbpos;
    list_for_each_entry(pbpos,&(prod->productionbody->list),list){
      pbody *pbody=pbpos->body;
      slist *position;
      list_for_each_entry(position,&(pbody->list),list){
	int key=(int)(position->key);
	if(key>=table->bias)
	  if(key!=head)
	    arraynodeaddedge(nodearr,key-table->bias,head-table->bias,nodenum);	
      }
    }
  }
  arraytopologysort(nodearr,nodenum,table->toposort);
  for(int i=0;i<table->count;i++)
    table->toposort[i]+=table->bias;
}
//========================print/test
void printproductionwithname(symboltable *table);

void printproductionwithname(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    symbolattr *attr=item->attr;
    production *prod=attr->attr.prod;
    int head=prod->head;
    item=searchsymboltablebyid(table,head);
    printf("head %s\n",item->name);
    productionbody *pbpos;
    list_for_each_entry(pbpos,&(prod->productionbody->list),list){
      pbody *pbody=pbpos->body;
      slist *position;
      list_for_each_entry(position,&(pbody->list),list){
	printf("  ");
	int body=(int)(position->key);
	if(body<table->bias)
	  printf("%c ",(char)body);
	else{
	  item=searchsymboltablebyid(table,body);
	  printf("%s ",item->name);
	}
      }
      printf("\n");
    }
    printf("\n");
  }
}
#endif
