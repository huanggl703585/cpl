#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H

#include <stdlib.h>
#include "symbolattr.h"
#include "hash.h"
#include "symboltableoption.h"

#define ASCII_BIAS 127

typedef struct symboltable symboltable;
typedef struct symbolitem symbolitem;

struct symbolitem{
  int id; //id starts from 0
  char *name;
  int derivecnt;
  symbolattr *attr;
};

struct symboltable{
  int size;
  int count; //count is the number of symbol, not include bias
  int bias; //for idarray
  int *idarray;
  int *toposort;
  int topofirst;
  symbolitem **table;   //it stores non-terminal
  charmapper **terminal;//it stores terminal
  symboltableoption option;
};

#define symbol_table_basic_size 1024

#define iterate_symbol_attr(table,func)					\
  do{									\
    for(int __i=0;__i<table->count;__i++){				\
      symbolitem *__item=searchsymboltablebyid(table,__i+table->bias);	\
      func(__item->attr);						\
    }									\
  }while(0)

symboltable* createsymboltable(size_t size,int bias);
int insertsymboltable(symboltable* st,char *str,symbolattr *attr);
symbolitem* searchsymboltablebyname(symboltable *st,char *str);
symbolitem* searchsymboltablebyid(symboltable *st,int id);
int changesymboltablebyname(symboltable *st,char *str,symbolattr *attr);
int changesymboltablebyid(symboltable *st,int id,symbolattr *attr);

symboltableoption *symboltablegetoption(symboltable *table);
void symboltablesetoption(symboltable *table,symboltableoption *option);

symboltable *createsymboltable(size_t size,int bias)
{
  symboltable* ret=(symboltable*)malloc(sizeof(symboltable));
  ret->size=size;
  ret->count=0;
  ret->bias=bias;
  ret->idarray=(int*)malloc(sizeof(int)*(ret->size));
  ret->toposort=(int*)malloc(sizeof(int)*(ret->size));
  ret->topofirst=0;
  ret->table=(symbolitem**)malloc(sizeof(symbolitem*)*(ret->size));
  ret->terminal=(charmapper**)malloc(sizeof(charmapper*)*(ret->bias));
  bzero(ret->idarray,sizeof(int)*(ret->size));
  bzero(ret->table,sizeof(symbolitem*)*(ret->size));
  bzero(ret->terminal,sizeof(charmapper*)*(ret->bias));
  bzero(ret->toposort,sizeof(int)*(ret->size));
  init_symboltable_option(ret->option);
  return ret;
}

int insertsymboltable(symboltable *st,char *str,symbolattr *attr)
{
  unsigned int hashvalue = strhash(str)%(st->size);
  unsigned int head=hashvalue;
  symbolitem *item=(st->table)[hashvalue];

  while(item!=NULL){
    if(strcmp(item->name,str)==0){
      //whether dupcate name is an error depends on table->option
      if(st->option.duplicate==1)
	return item->id;
      else
	return -1;
    }
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
  item->derivecnt=0;
  if(attr!=NULL)
    item->attr=attr;
  else
    item->attr=createsymbolattr();
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

symboltableoption *symboltablegetoption(symboltable *table)
{
  symboltableoption *ret=(symboltableoption*)malloc(sizeof(symboltableoption));
  memcpy(ret,&(table->option),sizeof(symboltableoption));
  return ret;
}

void symboltablesetoption(symboltable *table,symboltableoption *option)
{
  memcpy(&(table->option),option,sizeof(symboltableoption));
}

//===========================================
void disassembleor(symboltable *table)
{
  iterate_symbol_attr(table,_disassembleor);
}

//============================================
#define symbolderivename(newname,oriname,ulen)			\
  char *newname=(char*)malloc(strlen(oriname)+1+ulen);		\
  do{								\
    char padding[ulen+1];					\
    for(int i=0;i<ulen;i++) padding[i]='_';			\
    padding[ulen]='\0';						\
    sprintf(newname,"%s%s",padding,oriname);			\
  }while(0)

int derivenewsymbol(symboltable *table,symbolitem *origin)
{
  //symbolattr *attr=createsymbolattr();
  origin->derivecnt++;
  symbolderivename(newname,origin->name,origin->derivecnt);
  return insertsymboltable(table,newname,NULL);
}

//for one symbol
//it rely on symbolsettype
void extractleftlcpone(symboltable *table,int id)
{
  symbolitem *item=searchsymboltablebyid(table,id);
  if(item->attr->type==TERMINALSET)
   return ;
  production *prod=item->attr->attr.prod;
  int pcnt=prod->cnt;
  while(1){
    prodfindlcp(prod,mark,res,lcp,len,restlen);
    
    if(listisempty(lcp->list)){ //no any lcp
      break;
    }
    //we should promise that no empty symbol
    int hasempty=0;
    int newid=derivenewsymbol(table,item);
    symbolitem *newitem=searchsymboltablebyid(table,newid);
    newitem->attr->attr.prod=createproduction(newid);
    production *newprod=newitem->attr->attr.prod;
    productionbody *pbpos=prodbodynext(prod->productionbody);
    for(int i=0;i<pcnt;i++){
      productionbody *tmp=prodbodynext(pbpos);
      if(mark[i]==1){
	productiondrop(prod,pbpos);
	//printslist(pbpos->body);
	if(restlen[i]!=0){
	  productionbody *pb=createprodbodywithpbody(res[i],restlen[i]);
	  productionappend(newprod,pb);
	  //TODO: CLEAN ORIGIN PB
	}
	else
	  hasempty=1;
      }
      pbpos=tmp;
    }
    productionbody *lcppb=createprodbodywithpbody(lcp,len);
    if(hasempty==1){
      productionappend(prod,lcppb);
    }
    appendprodbody(lcppb,newid);
    productionappend(prod,lcppb);
  }
}

//UNTEST
void extractleftlcp(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    int id=i+table->bias;
    
    extractleftlcpone(table,id);
  }
}

//===============================toposort
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
  arraytopologysort(nodearr,table->count,table->toposort);
  for(int i=0;i<table->count;i++)
    table->toposort[i]+=table->bias;
}

//========================left recursion
void elimateleftrecursion(symboltable *table);

//it rely on extract left lcp
void elimateleftrecursion(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    production *prod=item->attr->attr.prod;
    productionbody *pbpos;
    //because we have extracted left lcp, so just have one left-recursion production
    list_for_each_entry(pbpos,&(prod->productionbody->list),list){
      if(prodhaveleftrecursion(prod,pbpos)==1){
	int newid=derivenewsymbol(table,item);
	symbolitem *newitem=searchsymboltablebyid(table,newid);
	newitem->attr->attr.prod=createproduction(newid);
	production *newprod=newitem->attr->attr.prod;
	pbody *head=getpbodynext(pbpos->body);
	int newpblen=pbpos->cnt-1;
	//printslist(head);
	//printslist(pbpos->body);
	//printf("newpblen %d ",newpblen);
	//printpbodyunit(pbody,newpblen);
	productionbody *newpb=createprodbodywithpbody(head,newpblen);
	appendprodbody(newpb,newid);
	productionappend(newprod,newpb);

	productionbody *tmp=prodbodynext(prod->productionbody);
	int pcnt=prod->cnt;
	for(int i=0;i<pcnt;i++){ 
	  if(tmp!=pbpos){
	    productionbody *copypb=createprodbodywithpbody(tmp->body,tmp->cnt);
	    productionappend(prod,copypb);
	    appendprodbody(tmp,newid);
	  }
	  tmp=prodbodynext(tmp);
	}
	
	productiondrop(prod,pbpos);
      }
    }
  }
}

//============================type,mapper
//solve left recursion before
void symbolsettype(symboltable *table);
void printsymbolattr(symboltable *table);

void symbolsetmapper(symboltable *table);
void _symbolsetmapper(symboltable *table,production *prod,int mapto);

#define havemapper(table,id)			\
  (table->terminal[id]!=NULL)

void symbolsettype(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    symbolattr *attr=item->attr;
    symbolsetattr(attr);
  }
}

void printsymbolattr(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    _printsymbolattr(item->attr);
  }
}
void symbolsetmapper(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    if(item->attr->type==TERMINALSET){
      production *prod=item->attr->attr.prod;
      _symbolsetmapper(table,prod,i+table->bias);
    }
  }
}

void _symbolsetmapper(symboltable *table,production *prod,int mapto)
{
  productionbody *pbpos;
  symbolitem *item;
  charmapper *mapper=NULL;
  list_for_each_entry(pbpos,&(prod->productionbody->list),list){
    pbody *body=getpbodynext((pbpos->body));
    int id=getpbodykey(body);
    if((table->terminal[id])==NULL){
      table->terminal[id]=createcharmapper(id);
    }
    charmapperappend(table->terminal[id],mapto);
  }
}
//========================re_exp
//TODO : IT SHOULD MOVE TO GRAMMAR.H
void prodsettoreexp(symboltable *table);
void symbolreexptransit(symboltable *table,re_exp *re,int changesign);
void printreexpset(symboltable *table); 

#define CHANGESIGN   1
#define UNCHANGESIGN 0
void prodsettoreexp(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,table->toposort[i]);
    symbolattr *attr=item->attr;
    if(attr->type==TERMINALSEQ || attr->type==NONTERMINAL){
      re_exp *re=productiontoreexp(attr->attr.prod,table->toposort[i]);
      //printreexp(re);
      if(attr->type==NONTERMINAL){
	if(table->toposort[i]==127)
	  symbolreexptransit(table,re,CHANGESIGN);
	else
	  symbolreexptransit(table,re,UNCHANGESIGN);
      }
      attr->reexp=re;
    }
  }
}

void symbolreexptransit(symboltable *table,re_exp *re,int changesign)
{
  re_exp *pos;
  list_for_each_entry(pos,&(re->list),list){
    if(pos->type==OPERAND){
      int id=pos->id;
      symbolitem *item=searchsymboltablebyid(table,id);
      if(item->attr->type==NONTERMINAL || item->attr->type==TERMINALSEQ){
	re_exp *replace=item->attr->reexp;
	if(changesign==CHANGESIGN){
	  reexpreplacesignunchange(pos,replace);
	}
	else
	  reexpreplace(pos,replace);
      }
    }
  }
}

//TODO
void printreexpset(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,table->toposort[i]);
    if(item->attr->type==NONTERMINAL || item->attr->type==TERMINALSEQ)
      printreexp(item->attr->reexp);
  }
}
//========================print/test
//option==0 print in id order
//option==1 print in toposort order
void printproductionwithname(symboltable *table,int option);

void printproductionwithname(symboltable *table,int option)
{
  for(int i=0;i<table->count;i++){
    int id;
    if(option==0)
      id=i+table->bias;
    else
      id=table->toposort[i];
    symbolitem *item=searchsymboltablebyid(table,id);
    symbolattr *attr=item->attr;
    production *prod=attr->attr.prod;
    int head=prod->head;
    item=searchsymboltablebyid(table,head);
    printf("head %s cnt %d\n",item->name,prod->cnt);
    productionbody *pbpos;
    list_for_each_entry(pbpos,&(prod->productionbody->list),list){
      printf("cnt %d\n",pbpos->cnt);
      pbody *body=pbpos->body;
      pbody *position;
      printf("  ");
      pbody_for_each(position,(pbpos->body)){
	int key=(int)(position->key);
	if(key=='\''){
	  pbody *pbodytmp=getpbodynext(position);
	  int tmpkey=getpbodykey(pbodytmp);
	  printf("%c ",(char)tmpkey);
	  position=(getpbodynext(pbodytmp));
	}
	else if(key<table->bias)
	  printf("%c ",(char)key);
	else{
	  item=searchsymboltablebyid(table,key);
	  printf("%s ",item->name);
	}
      }
      printf("\n");
    }
    printf("\n");
  }
}

#endif
