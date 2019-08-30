#ifndef __FORECASTTABLE_H
#define __FORECASTTABLE_H

#include <limits.h>

#define HEAD_ELEMENT INT_MAX

typedef struct foreitem foreitem;
typedef struct forelist forelist;
struct foreitem{
  int input;
  productionbody *body;
  struct list_head list;
};
struct forelist{
  int nonterminal;
  foreitem *col;  //col link each foreitem, which prodbody's head is
                  //forelist's nonterminal
  struct list_head row;  //row link each forelist
};

#define for_each_forelist(pos,listhead)		\
  list_for_each_entry(pos,&(listhead->row),row)

#define forelist_addtail(new,pos)		\
  listaddtail(&(new->row),&(pos->row))

#define forelist_next(pos)			\
  list_next_entry(&(pos->row),forelist,row)

#define for_each_foreitem(pos,listhead)		\
  list_for_each_entry(pos,&(listhead->list),list)

#define foreitem_addtail(new,pos)		\
  listaddtail(&(new->list),&(pos->list))

foreitem *createForeitem(int input,producionbody *prodbody);
forelist *createForelist(int nonterminal);
forelist *createForetable();

void insertForelist(forelist *list,int input,productionbody *body);
void insertForetable(forelist *table,int head,int input,productionbody *body);
productionbody *findForetable(forelist *table,int rindex,int cindex);

void printForetable(forelist *table);

foreitem *createForeitem(int input,producionbody *prodbody)
{
  foreitem *ret=(foreitem*)malloc(sizeof(foreitem));
  ret->input=input;
  ret->body=prodbody;
  list_init(ret->list);
  return ret;
}

forelist *createForelist(int nonterminal)
{
  forelist *ret=(forelist*)malloc(sizeof(forelist));
  ret->nonterminal=nonterminal;
  ret->col=createForeitem(HEAD_ELEMENT,NULL);
  list_init(ret->row);
  return ret;
}

forelist *createForetable()
{
  return createForelist(HEAD_ELEMENT);
}

void insertForelist(forelist *list,int input,productionbody *body)
{
  foreitem *newitem=createForeitem(input,body);
  foreitem *pos;
  for_each_foreitem(pos,(list->col)){
    if(pos->input==input) assert(0); //duplicate is an error
    if(pos->input>input) break;
  }
  foreitem_addtail(newitem,pos);
}

void insertForetable(forelist *table,int head,int input,productionbody *body)
{
  forelist *pos;
  for_each_forelist(pos,table){
    if(pos->nonterminal==head) break;
    if(pos->nonterminal>head){  //insert new forelist
      forelist *newlist=createForelist(head);
      forelist_addtail(newlist,pos);
      pos=newlist;
      break;
    }
  }
  insertForelist(pos,input,body);
}

productionbody *findForetable(forelist *table,int rindex,int cindex)
{
  forelist *listpos;
  for_each_forelist(listpos,table){
    if(listpos->nonterminal==rindex)    break;
    if(listpos->nonterminal >rindex)    return NULL;
  }
  if(listpos->nonterminal!=rindex) return NULL;
  
  foreitem *itempos;
  for_each_foreitem(itempos,(listpos->row)){
    if(itempos->input==cindex)     break;
    if(itempos->input >cindex)     return NULL;
  }
  if(itempos->input==cindex) return itempos->body;
  return NULL;
}

void printForetable(forelist *table)
{
  forelist *listpos;
  for_each_forelist(listpos,table){
    int x=listpos->nonterminal;
    foreitem *itempos;
    for_each_foreitem(itempos,(listpos->col)){
      int y=itempos->input;
      printf("%d %d\t",x,y);
      printPbodyunit(itempos->body->unit);
      printf("\n");
    }
  }
} 
#endif
