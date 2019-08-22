#ifndef __JUMPTABLE_LINK_H
#define __JUMPTABLE_LINK_H

#include "list.h"
#include <stdlib.h>

typedef struct jumptable_link jumptable_link;
typedef struct statenode statenode;
typedef struct jumpnode jumpnode;
 
struct jumptable_link{
  int statecnt;
  statenode* statelist;
};

//stateindex starts from 1,
//because state 0 remains as exception condition

struct statenode{
  int stateid;
  int jumpcnt;
  int mininput;
  int maxinput;
  struct list_head statelist;
  jumpnode* jumplist;
};

struct jumpnode{
  int input;
  int dest;
  struct list_head jumplist;
};

jumptable_link* createjumptablelink();
int jumptableadd(jumptable_link *jtable,int from,int inupt,int to);
int jumptablefind(jumptable_link *jtable,int from,int input);

void printjumptable(jumptable_link *jtable);

//sum of each statenode's maxinput-mininput+1
int getjumpsum(jumptable_link *jtable);

jumptable_link* createjumptablelink()
{
  jumptable_link* ret=(jumptable_link*)malloc(sizeof(jumptable_link));
  ret->statecnt=0;
  
  statenode *listhead=(statenode*)malloc(sizeof(statenode));
  listhead->stateid=0;
  list_init(listhead->statelist);
  ret->statelist=listhead;

  return ret;
}

static statenode* createstatenode(jumptable_link* jtable,int stateid);
static jumpnode* createjumpnode(statenode *snode,int input,int dest);

static statenode* createstatenode(jumptable_link* jtable,int stateid)
{
  statenode *ret=(statenode*)malloc(sizeof(statenode));
  ret->stateid=stateid;
  ret->jumpcnt++;
  ret->mininput=ret->maxinput=0;
  list_init(ret->statelist);
  
  list_insert_order(jtable->statelist,statelist,stateid,ret,intcmp);
  
  jumpnode *listhead=(jumpnode*)malloc(sizeof(jumpnode));
  listhead->input=0;
  list_init(listhead->jumplist);
  ret->jumplist=listhead;
  
  return ret;
}

static jumpnode* createjumpnode(statenode *snode,int input,int dest)
{
  jumpnode *ret=(jumpnode*)malloc(sizeof(jumpnode));
  ret->input=input;
  ret->dest=dest;
  list_init(ret->jumplist);
  
  list_insert_order(snode->jumplist,jumplist,input,ret,intcmp);
  
  return ret;
}

int jumptableadd(jumptable_link *jtable,int from,int in,int to)
{
  statenode *pos,*pos2;
  list_find_key(pos,jtable->statelist,statelist,stateid,from,intequal);
  if(pos==jtable->statelist){ //no such "from" statenode
    pos=createstatenode(jtable,from);
    jtable->statecnt++;
  }

  list_find_key(pos2,jtable->statelist,statelist,stateid,to,intequal);
  if(pos2==jtable->statelist){
    createstatenode(jtable,to);
    jtable->statecnt++;
  }

  jumpnode *jpos;
  list_find_key(jpos,pos->jumplist,jumplist,input,in,intequal);
  if(jpos==pos->jumplist){ //no dup jump item
    if(pos->mininput==0) //first insert, so change the mininput&maxinput both
      pos->mininput=pos->maxinput=in;
    else
      keepinterval(pos->mininput,pos->maxinput,in);
    
    createjumpnode(pos,in,to);
    pos->jumpcnt++;
  }
  else{ //we consider dup condition as exception
    return 1;
  }
  return 0;
}

int jumptablefind(jumptable_link *jtable,int from,int in)
{
  statenode *pos;
  list_find_key(pos,jtable->statelist,statelist,stateid,from,intequal);
  if(pos==jtable->statelist) //no such from node
    return 0;

  jumpnode *jpos;
  list_find_key(jpos,pos->jumplist,jumplist,input,in,intequal);
  if(jpos==pos->jumplist)
    return 0;

  return jpos->dest;
}

int getjumpsum(jumptable_link *jtable)
{
  int sum=0;
  statenode *pos;
  statenode *head=jtable->statelist;
  list_for_each_entry(pos,&(head->statelist),statelist){
    //printf("%d %d %d\n",pos->stateid,pos->maxinput,pos->mininput);
    sum+=(pos->maxinput-pos->mininput+1);
  }
  return sum;
}

void printjumptable(jumptable_link *jtable)
{
  printf("jumptable statecnt %d\n",jtable->statecnt);
  
  statenode *siter;
  statenode *shead=jtable->statelist;
  list_for_each_entry(siter,&(shead->statelist),statelist){
    printf("stateid %d\n",siter->stateid);
    jumpnode *jiter;
    jumpnode *jhead=siter->jumplist;
    list_for_each_entry(jiter,&(jhead->jumplist),jumplist){
      printf(" %d %d\n",jiter->input,jiter->dest);
    }
  }
  printf("\n");
}

#endif
