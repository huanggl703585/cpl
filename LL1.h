#ifndef __BUILDFIRST_H
#define __BUILDFIRST_H

#include "symboltable.h"

//=======================build first set
void buildfirst(symboltable *table);
//return 1 if the firstset has added new
int buildfirstone(symboltable *table,int id);
//0 stands for empty
int firstsethaveempty(symboltable *table,int id);

void buildfirst(symboltable *table)
{
  //int mark[table->count];
  //bzero(mark,sizeof(int)*table->count);
  int contine=1;
  while(contine){
    contine=0;
    for(int i=0;i<table->count;i++){
      int tmp=buildfirstone(table,i+table->bias);
      //mark[i]=tmp;
      if(tmp!=0) contine=1;
    }
    //for(int i=0;i<table->count;i++)
    //  printf("%d ",mark[i]);
    //printf("\n");
    //bzero(mark,sizeof(int)*table->count);
  }
}

int buildfirstone(symboltable *table,int id)
{
  int ret=0;
  symbolitem *item=searchsymboltablebyid(table,id);
  set *target=item->attr->first;
  production *prod=item->attr->attr.prod;
  productionbody *pbpos;
  prodbody_for_each(pbpos,prod){
    pbody *pos;
    pbody_for_each(pos,(pbpos->body)){
      int key=getpbodykey(pos);
      if(key<table->bias){
	int tmp=insertset(target,key);
	item->attr->first=target;
	//printset(target);
	//printf("tmp %d\n",tmp);
	if(ret==0 && tmp!=0) ret=1;
	break;
      }
      else{
	symbolitem *srcitem=searchsymboltablebyid(table,key);
	set *src=srcitem->attr->first;
	int tmp=intersectset(&target,src);
	item->attr->first=target;
	if(ret==0 && tmp!=0) ret=1;
	if(firstsethaveempty(table,key)!=0)
	  break;
      }
    }
  }
  return ret;
}

int firstsethaveempty(symboltable *table,int id)
{
  symbolitem *item=searchsymboltablebyid(table,id);
  set *firstset=item->attr->first;
  return findset(firstset,0);
}

//=============================build follow set

void buildfollow(symboltable *table,int startid);
int _buildfollow(symboltable *table,int id);

void buildfollow(symboltable *table,int startid)
{
  symbolitem *startitem=searchsymboltablebyid(table,startid);
  //we use 0 to stands for '$'
  insertset((startitem->attr->follow),0);
  
  //int mark[table->count];
  //bzero(mark,sizeof(int)*table->count);

  int contine=1;
  while(contine){
    contine=0;
    for(int i=0;i<table->count;i++){
      int tmp=_buildfollow(table,i+table->bias);
      //mark[i]=tmp;
      if(contine==0 && tmp!=0) contine=1;
    }
    //for(int i=0;i<table->count;i++)
    //  printf("%d ",mark[i]);
    //printf("\n");
    //bero(mark,sizeof(int)*table->count);
  }
}

int _buildfollow(symboltable *table,int id)
{
  int ret=0;
  symbolitem *item=searchsymboltablebyid(table,id);
  set *headfollow=item->attr->follow;
  production *prod=item->attr->attr.prod;
  productionbody *pbpos;
  prodbody_for_each(pbpos,prod){
    pbody *p1,*p2;
    p1=getpbodynext(pbpos->body);
    p2=getpbodynext(p1);
    //book 4.4.2 case 2
    while(p2!=(pbpos->body)){
      int key1=getpbodykey(p1),key2=getpbodykey(p2);
      set *target;
      if(key1>=table->bias){
	symbolitem *item1=searchsymboltablebyid(table,key1);
	target=item1->attr->follow;
	int tmp=0;
	if(key2<table->bias){ //terminal's first is itself
	  tmp=insertset(target,key2);
	  if(ret==0 && tmp!=0) ret=1;
	}
	else{
	  symbolitem *item2=searchsymboltablebyid(table,key2);
	  set *src=item2->attr->follow;
	  tmp=intersectset(&target,src);
	  if(ret==0 && tmp!=0) ret=1;
	  //book 4.4.2 case 3
	  if(findset(src,0)){
	    tmp=intersectset(&target,headfollow);
	    if(ret==0 && tmp!=0) ret=1;
	  }
	}
	item1->attr->follow=target;
      }
      p1=getpbodynext(p1);
      p2=getpbodynext(p2);
    }
    //book 4.4.2 case 3
    int key1=getpbodykey(p1);
    if(key1>=table->bias){
      symbolitem *item1=searchsymboltablebyid(table,key1);
      set *target=item1->attr->follow;
      int tmp=intersectset(&target,headfollow);
      if(ret==0 && tmp!=0) ret=1;
      item1->attr->follow=target;
    }
  }
  return ret;
}

void printfirstandfollow(symboltable *table);

void printfirstandfollow(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    set *first=item->attr->first;
    set *follow=item->attr->follow;
    printset(first);
    printset(follow);
    printf("\n");
  }
}

//===========================forecast table


forecasttable* buildforecasttable(symboltable *table);

forecasttable* buildforecasttable(symboltable *table)
{
  forecasttable *ret;
  
}
#endif
