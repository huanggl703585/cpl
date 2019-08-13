#ifndef __PUNITLIST_H
#define __PUNITLIST_H

#include "pbodyunit.h"

typedef struct punit punit;
struct punit{
  pbodyunit *unit;
  struct list_head list;
};

#define punit_first(listhead)				\
  list_first_entry((&(listhead->list)),punit,list)

#define punit_prev(pos)				\
  list_prev_entry((&(pos->list)),punit,list)

#define for_each_punit(pos,listhead)		\
  list_for_each_entry(pos,&(listhead->list),list)

#define punit_insert(pos,new)			\
  lsitadd(&(new->list),&(pos->list))

punit *createpunit();
void punitappend(punit *listhead,pbodyunit *unit);
void printpunit(punit *listhead);
punit *createpunitbypbody(pbody *body);
void _expandpunitnesting(punit *pos);
void expandpunitnesting(punit *head);

punit *createpunit()
{
  punit *ret=(punit*)malloc(sizeof(punit));
  ret->unit=NULL;
  list_init(ret->list);
  return ret;
}

void punitappend(punit *listhead,pbodyunit *unit)
{
  punit *newunit=createpunit();
  newunit->unit=unit;
  listaddtail(&(newunit->list),&(listhead->list));
}

void printpunit(punit *listhead)
{
  punit *pos;
  for_each_punit(pos,listhead){
    printpbodyunit(pos->unit);
  }
  printf("\n");
}

punit *createpunitbypbody(pbody *body)
{
  punit *head=createpunit();
  pbody *pos=body;
  pbody *posnext;
  do{
    pbody *tmp=pos;
    pbodyunit *content=pbodygetunit(tmp,body,&pos);
    punitappend(head,content);
    posnext=getpbodynext(pos);
  }while(posnext!=body);
  return head;
}

void expandpunitnesting(punit *head)
{
  punit *pos;
  for_each_punit(pos,head){
    pbodyunit *u=pos->unit;
    if(u->type==P_COMBINE){
      _expandpunitnesting(pos);
    }
  }
}

void _expandpunitnesting(punit *pos)
{
  punit *posprev=punit_prev(pos);
  pbody *head=pos->unit->body;
  pbody *ppos=getpbodynext(head);
  do{
    pbodyunit *tmpunit=pbodygetunit(ppos,head,&ppos);
    punit *tmp=createpunit();
    punit->unit=tmpunit;
    punitinsert(prevpos,tmp);
    prevpos=tmp;

    pbody *pposnext=getpbodynext(ppos);
    int pposnextkey=getpbodykey(pposnext);
  }while(pposnextkey!=')');
}
#endif 
