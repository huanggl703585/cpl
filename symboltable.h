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
  ret->topofirst=0;
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
    
    if(listisempty(lcp->list)){
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
	printslist(pbpos->body);
	if(restlen[i]!=0){
	  productionbody *pb=createprodbodywithpbody(res[i],restlen[i]);
	  productionappend(newprod,pb);
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

//========================re_exp
//solve left recursion before
void symbolsettype(symboltable *table);
void prodsettoreexp(symboltable *table);
void symbolreexptransit(symboltable *table,re_exp *re);
void printreexpset(symboltable *table); 

void symbolsettype(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    symbolattr *attr=item->attr;
    symbolsetattr(attr);
  }
}

void prodsettoreexp(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,table->toposort[i]);
    symbolattr *attr=item->attr;
    if(attr->type==TERMINALSEQ || attr->type==NONTERMINAL){
      re_exp *re=productiontoreexp(attr->attr.prod);
      printreexp(re);
      if(attr->type==NONTERMINAL)
	symbolreexptransit(table,re);
      attr->reexp=re;
    }
  }
}

void symbolreexptransit(symboltable *table,re_exp *re)
{
  re_exp *pos;
  list_for_each_entry(pos,&(re->list),list){
    if(pos->type==OPERAND){
      int id=pos->id;
      symbolitem *item=searchsymboltablebyid(table,id);
      if(item->attr->type==NONTERMINAL || item->attr->type==TERMINALSEQ){
	re_exp *replace=item->attr->reexp;
	reexpreplace(pos,replace);
      }
    }
  }
}

void printreexpset(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,table->toposort[i]);
    printreexp(item->attr->reexp);
  }
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
    printf("head %s cnt %d\n",item->name,prod->cnt);
    productionbody *pbpos;
    list_for_each_entry(pbpos,&(prod->productionbody->list),list){
      printf("cnt %d\n",pbpos->cnt);
      pbody *pbody=pbpos->body;
      slist *position;
      printf("  ");
      list_for_each_entry(position,&(pbody->list),list){
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
