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

//==========================================
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

//===========================================
void reformproduction(symboltable *table);

void prodinunit(symboltable *table);
void disassembleor(symboltable *table);
void elimateparenthese(symboltable *table);
void elimateor(symboltable *table);
void extractleftlcpone(symboltable *table,symbolitem *item);
void extractleftlcp(symboltable *table);
void elimateleftrecursionone(symboltable *table,symbolitem *item);
void elimateleftrecursion(symboltable *table);
void printtablepunit(symboltable *table);

void reformproduction(symboltable *table)
{
  prodinunit(table);
  elimateparenthese(table);
  elimateor(table);
  extractleftlcp(table);
  elimateleftrecursion(table);
}

void prodinunit(symboltable *table)
{
  iterate_symbol_attr(table,_prodinunit);
}

void printtablepunit(symboltable *table)
{
  //iterate_symbol_attr(table,_printtablepunit);  
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    printf("%s %d\n",item->name,item->id);
    _printtablepunit(item->attr);
  }
}

void elimateparenthese(symboltable *table)
{
  iterate_symbol_attr(table,_elimateparenthese);
}

void elimateor(symboltable *table)
{
  iterate_symbol_attr(table,_elimateor);
}

void extractleftlcpone(symboltable *table,symbolitem *item)
{
  production *prod=item->attr->attr.prod;
  int pt;
  //the loop continue until there no other common prefix
  while(1){
    int size=prod->cnt;
    int mark[size];
    pbodyunit* unitarr[size];
    productionbody *pbpos;
    pt=0;
    prod_for_each_prodbody(pbpos,prod)
      unitarr[pt++]=pbpos->unit;
    pbodyunit *lcphead;
    int headindex;

    if(pbodyunitfindlcp(unitarr,mark,size,&lcphead,&headindex)==0)
      return ;
  
    int newid=derivenewsymbol(table,item);
    symbolitem *newitem=searchsymboltablebyid(table,newid);
    production *newprod=newitem->attr->attr.prod;
    pbodyunit *newunit=createpbodyunit();
    newunit->type=P_NONTERMINAL;newunit->value.index=newid;
    pbodyunit *lcp=pbodyunitcopy(lcphead,unitarr[headindex]);
    //printpbodyunit(lcp);
    //A-> alpha A` | beta
    //add new to A
    productionbody *originnewbody=createprodbodylinkprod(prod);
    originnewbody->unit=lcp;
    prodbodyappendpbodyunit(originnewbody,newunit);
    //delete A's prod, add prod to A`
    pbpos=prod->productionbody;
    productionbody *pbtmp=prodbodynext(pbpos);
    for(pt=0;pt<size;pt++){
      pbpos=pbtmp;
      pbtmp=prodbodynext(pbtmp);
      if(mark[pt]!=0){
	productionbody *tmppb=createprodbodylinkprod(newprod);
	prodbodyappendpbodyunitlist(tmppb,unitarr[pt]);
	productiondrop(prod,pbpos);
      }
    }
  }
}

void extractleftlcp(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    extractleftlcpone(table,item);
  }
}

void elimateleftrecursionone(symboltable *table,symbolitem *item)
{
  production *prod=item->attr->attr.prod;
  int head=prod->head;
  int pcnt=prod->cnt;
  int mark[pcnt];
  pbodyunit *unitarr[pcnt];
  productionbody *pbpos;
  int pt=0;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *tmp=pbpos->unit;
    unitarr[pt++]=tmp;
  }

  if(!(pbodyunitfindleftrecursion(unitarr,mark,pcnt,head)))
    return ;

  int newid=derivenewsymbol(table,item);
  symbolitem *newitem=searchsymboltablebyid(table,newid);
  production *newprod=newitem->attr->attr.prod;
  pbodyunit *newunit=createpbodyunit();
  newunit->type=P_NONTERMINAL;newunit->value.index=newid;
  pt=0;
  prod_for_each_prodbody(pbpos,prod){
    if(mark[pt]==0)
      prodbodyappendpbodyunit(pbpos,newunit);
    else{
      productionbody *tmppb=createprodbodylinkprod(newprod);
      pbodyunit *liststart=pbodyunitnext(unitarr[pt]);
      prodbodyappendpbodyunitlist(tmppb,liststart);
      prodbodyappendpbodyunit(tmppb,newunit);
      
      productiondrop(prod,pbpos);
    }
    pt++;
  }
  productionbody *emptyprod=createprodbodylinkprod(newprod);
  pbodyunit *empty=createpbodyunitempty();
  prodbodyappendpbodyunit(emptyprod,empty);
}

void elimateleftrecursion(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    elimateleftrecursionone(table,item);
  }
}

//============================type,mapper
//solve left recursion before
void symbolsettype(symboltable *table);
void printsymbolattr(symboltable *table);

void symbolsetmapper(symboltable *table);
void _symbolsetmapper(symboltable *table,production *prod);
void printsymbolmapper(symboltable *table);

#define havemapper(table,id)			\
  (table->terminal[id]!=NULL)

void symbolsettype(symboltable *table)
{
  iterate_symbol_attr(table,_symbolsettype);
}

void printsymboltype(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    printf("%s\t",item->name);
    _printsymboltype(item->attr);
  }
}

void symbolsetmapper(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    if(item->attr->type==S_TERMINALSET){
      production *prod=item->attr->attr.prod;
      _symbolsetmapper(table,prod);
    }
  }
}

void _symbolsetmapper(symboltable *table,production *prod)
{
  int mapto=prod->head;
  productionbody *pbpos;
  symbolitem *item;
  charmapper *mapper=NULL;
  prod_for_each_prodbody(pbpos,prod){
    pbodyunit *u=pbpos->unit;
    u=pbodyunitnext(u);
    int value=u->value.index;
    if(table->terminal[value]==NULL)
      table->terminal[value]=createcharmapper(value);
    charmapperappend(table->terminal[value],mapto);
  }
}

void printsymbolmapper(symboltable *table)
{
  for(int i=0;i<table->bias;i++){
    charmapper *mapper=table->terminal[i];
    if(mapper!=NULL)
      printcharmapper(table->terminal[i]);
  }
}

//===============================toposort
void pbodyunitaddedge(pbodyunit *list,node nodearr[],int *nodenum,int head,int bias);
void symboltoposort(symboltable *table);

void pbodyunitaddedge(pbodyunit *list,node nodearr[],int *nodenum,int head,int bias)
{
  pbodyunit *pos;
  for_each_pbodyunit(pos,list){
    if(pos->type==P_NONTERMINAL){
      int key=pos->value.index;
      if(key!=head){
	int num=*nodenum;
	arraynodeaddedge(nodearr,key-bias,head-bias,num);
	*nodenum=num;
      }
    }
    else if(pos->type==P_COMBINE){
      pbodyunit *nest=pos->value.nest;
      pbodyunitaddedge(nest,nodearr,nodenum,head,bias);
    }
  }
}
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
    if(item->attr->type!=S_NONTERMINAL) continue;

    production *prod=item->attr->attr.prod;
    int head=prod->head;
    productionbody *pbpos;
    prod_for_each_prodbody(pbpos,prod){
      pbodyunit *unitlist=pbpos->unit;
      pbodyunitaddedge(unitlist,nodearr,&nodenum,head,table->bias);
    }
  }
  arraytopologysort(nodearr,table->count,table->toposort);
  for(int i=0;i<table->count;i++)
    table->toposort[i]+=table->bias;
}

//========================re_exp

//return a expanded retree
re_node* symboltablebuildretree(symboltable *table,int start);
//combine until start symbol, because we need to mark other symbol
void symboltableretreecombine(symboltable *table,int start);
void dosymboltableretreecombine(symboltable *table,re_node *root);
void symboltableretreecopycombine(symboltable *table,re_node *root);
void symboltableretreesetmark(symboltable *table,int start);
void symboltableprintretree(symboltable *table);

re_node* symboltablebuildretree(symboltable *table,int start)
{
  iterate_symbol_attr(table,_symboltablebuildretree);
  symboltableretreecombine(table,start);
  symboltableretreesetmark(table,start);
  
  symbolitem *startitem=searchsymboltablebyid(table,start);
  re_node *starttree=startitem->attr->attr.prod->retree;
  symboltableretreecopycombine(table,starttree);
  re_node *ret=retreeexpand(starttree,0);
  retreecount(ret);
  return ret;
}

//combine every symbol's retree except start symbol
//for example, if production is token ::= identifier | equivalence
//then the retree of identifier and equivalence is combined by reference
//the retree of start symbol should be combined by copy
void symboltableretreecombine(symboltable *table,int start)
{
  for(int i=0;i<table->count;i++){
    if(table->toposort[i]==start) continue;
    symbolitem *item=searchsymboltablebyid(table,table->toposort[i]);
    if(item->attr->type==S_TERMINALSET) continue;

    dosymboltableretreecombine(table,item->attr->attr.prod->retree);
  }
}

void dosymboltableretreecombine(symboltable *table,re_node *root)
{
  re_node *left=root->left,*right=root->right;
  if(left!=NULL && left->type==RE_OPERAND && left->value>table->bias){
    symbolitem *newitem=searchsymboltablebyid(table,left->value);
    re_node *newtree=newitem->attr->attr.prod->retree;
    retreereplace(root,isleft,newtree);
  }
  if(right !=NULL && right->type==RE_OPERAND && right->value>table->bias){
    symbolitem *newitem=searchsymboltablebyid(table,right->value);
    re_node *newtree=newitem->attr->attr.prod->retree;
    retreereplace(root,isright,newtree);
  }
  if(left!=NULL && left->type==RE_OPERATOR)
    dosymboltableretreecombine(table,left);
  if(right!=NULL && right->type==RE_OPERATOR)
    dosymboltableretreecombine(table,right);
}

//special for start symbol
void symboltableretreecopycombine(symboltable *table,re_node *root)
{
  re_node *left=root->left,*right=root->right;
  re_node *newtree;
  if(left!=NULL && left->type==RE_OPERAND){
    if(left->value>table->bias){
      symbolitem *newitem=searchsymboltablebyid(table,left->value);
      newtree=retreecopy(newitem->attr->attr.prod->retree);
    }
    else
      newtree=renodecopy(left);
    newtree=retreeexpand(newtree,newtree->mark);
    retreereplace(root,isleft,newtree);
  }
  if(right !=NULL && right->type==RE_OPERAND){
    if(right->value>table->bias){
      symbolitem *newitem=searchsymboltablebyid(table,right->value);
      newtree=retreecopy(newitem->attr->attr.prod->retree);
    }
    else
      newtree=renodecopy(right);
    newtree=retreeexpand(newtree,newtree->mark);
    retreereplace(root,isright,newtree);
  }
  if(left!=NULL && left->type==RE_OPERATOR)
    symboltableretreecopycombine(table,left);
  if(right!=NULL && right->type==RE_OPERATOR)
    symboltableretreecopycombine(table,right);
}

void symboltableretreesetmark(symboltable *table,int start)
{
  symbolitem *item=searchsymboltablebyid(table,start);
  re_node *root=item->attr->attr.prod->retree;
  re_node *stack[1024];
  int pt=0;
  stack[pt++]=root;
  while(--pt>=0){
    re_node *curnode=stack[pt];
    if(curnode->type==RE_OPERAND){
      if(curnode->value>table->bias){
	symbolitem *curitem=searchsymboltablebyid(table,curnode->value);
	retreesetmark(curitem->attr->attr.prod->retree,curnode->value);
      }
      else
	curnode->mark=curnode->value;
    }
    if(curnode->left!=NULL) stack[pt++]=curnode->left;
    if(curnode->right!=NULL) stack[pt++]=curnode->right;
  }
}

void symboltableprintretree(symboltable *table)
{
  for(int i=0;i<table->count;i++){
    symbolitem *item=searchsymboltablebyid(table,i+table->bias);
    if(item->attr->type==S_TERMINALSET) continue;

    printretree(item->attr->attr.prod->retree);
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
    prod_for_each_prodbody(pbpos,prod){
      printf("cnt %d\n",pbpos->cnt);
      _printpbodyunit(pbpos->unit);
    }
    printf("\n");
  }
}

#endif
