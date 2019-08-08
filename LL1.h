#ifndef __BUILDFIRST_H
#define __BUILDFIRST_H

#include "sliststack.h"
#include "symboltable.h"
#include "tokenlist.h"
#include "ast.h"

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
	if(srcitem->attr->type==TERMINALSET){
	  int tmp=insertset(target,key);
	  item->attr->first=target;
	  if(ret==0 && tmp!=0) ret=1;
	  break;
	}
	else{
	  set *src=srcitem->attr->first;
	  int tmp=intersectset(&target,src);
	  item->attr->first=target;
	  if(ret==0 && tmp!=0) ret=1;
	  if(firstsethaveempty(table,key)!=0)
	    break;
	}
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
	  if(item2->attr->type==TERMINALSET)
	    tmp=insertset(target,key2);
	  else{
	    set *src=item2->attr->follow;
	    tmp=intersectset(&target,src);	  
	    if(ret==0 && tmp!=0) ret=1;
	    //book 4.4.2 case 3
	    if(findset(src,0))
	      tmp=intersectset(&target,headfollow);
	  }
	  if(ret==0 && tmp!=0) ret=1;
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

typedef kvpair forecastitem;
//key is a terminal index, value is pbody

#define createforecastitem(key,value)			\
  (forecastitem*)_createkvpair((void*)key,(void*)value)

#define initforecastlist(sitem)				\
  do{							\
    if(sitem->attr->type==TERMINALSET)			\
      sitem->attr->forecastlist=createdarray(10,2);	\
    else						\
      sitem->attr->forecastlist=createdarray(2,2);	\
  }while(0)
  
void buildforecasttable(symboltable *table);
void _buildforecasttable(symboltable *table,int head,pbody *body);

int forecastcmp(void *a,void *b);
//M[X,a], X is argument first, a is argument second
pbody *findforecasttable(symboltable *table,int first,int second);

void _printforecastlist(darray *darr);
void printforecasttable(symboltable *table);

void buildforecasttable(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    int head=i+table->bias;
    symbolitem *item=searchsymboltablebyid(table,head);
    initforecastlist(item);
    production *prod=item->attr->attr.prod;
    productionbody *pbpos;
    prodbody_for_each(pbpos,prod){
      pbody *body=pbpos->body;
      _buildforecasttable(table,head,body);
    }
  }
}

void _buildforecasttable(symboltable *table,int head,pbody *body)
{
  symbolitem *item=searchsymboltablebyid(table,head);
  darray *darr=item->attr->forecastlist;
  pbody *pos;
  pbody *firstpos=getpbodynext(body);
  int firstsymbol=getpbodykey(firstpos);
  if(firstsymbol<table->bias){
    forecastitem *fitem=createforecastitem(firstsymbol,body);
    sortinsertdarray(darr,fitem,intkvpaircmp);
  }
  else{
    symbolitem *sitem=searchsymboltablebyid(table,firstsymbol);
    set *firstset=sitem->attr->first;
    //TODO:SEE BOOK p143 CASE 2
    if(findset(firstset,0)){

    }
    int recver[256];
    int recvercnt;
    travelavltree(firstset,recver,256,recvercnt);
    for(int i=0;i<recvercnt;i++){
      int key=(int)(recver[i]);
      if(key<table->bias){
	forecastitem *fitem=createforecastitem(key,body);
	sortinsertdarray(darr,fitem,intkvpaircmp);
      }
    }
  }
}

pbody *findforecasttable(symboltable *table,int first,int second)
{
  symbolitem *item=searchsymboltablebyid(table,first);
  darray *forecastlist=item->attr->forecastlist;
  void *ret=sortfinddarray(forecastlist,second,forecastcmp);
  if(ret==NULL) return NULL;
  return (pbody*)(((forecastitem*)ret)->value);
}

int forecastcmp(void *a,void *b)
{
  forecastitem *_a=(forecastitem*)a;
  forecastitem *_b=(forecastitem*)b;
  int ka=(int)(_a->key),kb=(int)(_b->key);
  return intcmp(ka,kb);
}

void printforecasttable(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    darray *darr=item->attr->forecastlist;
    _printforecastlist(darr);
  }
}

void _printforecastlist(darray *darr)
{
  for(int i=0;i<darr->pt;i++){
    forecastitem *fitem=finddarray(darr,i);
    printf("%d ",(int)(fitem->key));
    printslist((fitem->value));
  }
  printf("\n");
} 

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
#endif
