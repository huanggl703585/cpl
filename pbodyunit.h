#ifndef __PBODYUNIT_H
#define __PBODYUNIT_H

#include "pbody.h"

#include <stdio.h>

#define P_NONTERMINAL 1  //a symbol, like identifier, token
#define P_TERMINAL    2  //a terminal, like 'a', '|'
#define P_COMBINE     3  //a group of symbol in a pair of parentheses, liks (ab)
#define P_OR          4  // |

typedef struct pbodyunit pbodyunit;
struct pbodyunit{
  pbody *body;
  union{
    int index; //for P_TERMINAL \ P_NONTERMINAL
    pbodyunit *nest;  //for P_COMBINE
  }value;
  int type;
  struct list_head list;
};

#define for_each_pbodyunit(pos,listhead)	\
  list_for_each_entry(pos,&(listhead->list),list)

#define pbodyunitappend(newlist,listhead)			\
  listaddtail(&(newlist->list),&(listhead->list))

#define pbodyunitinsert(newlist,pos)		\
  listadd(&(newlist->list),&(pos->list))

#define pbodyunitdrop(pos)			\
  listdrop(&(pos->list))

#define pbodyunitprev(pos)			\
  list_prev_entry(pos,list)			

#define pbodyunitnext(pos)			\
  list_next_entry(pos,list)

#define pbodyunitishead(pos)			\
  (pos->type==0)

#define pbodyunitreplace(newhead,replacedpos)	\
  listreplace(&(newhead->list),&(replacedpos->list))

pbodyunit *createpbodyunit();
pbodyunit *getunitbypbody(pbody *head,pbody *end);
pbodyunit *_getunitbypbody(pbody *head,pbody *end,pbody **newhead);

//int pbodyunitequal(pbodyunit *u1,pbodyunit *u2);

void printpbodyunittype(int type);
void _printpbodyunit(pbodyunit *u);
void printpbodyunit(pbodyunit *listhead);

int pcombinehasor(pbodyunit *pos);
int pcombinehasalign(pbodyunit *pos);
void pcombineresolve(pbodyunit *pos);
int _pbodyunitelimateparenthese(pbodyunit *pos);
void pbodyunitelimateparenthese(pbodyunit *listhead);

int _testpbodyunitelimateor(pbodyunit *listhead);

int pbodyunitisequal(pbodyunit *u1,pbodyunit *u2);

pbodyunit *createpbodyunit()
{
  pbodyunit *ret=(pbodyunit*)malloc(sizeof(pbodyunit));
  list_init(ret->list);
  ret->value.index=0;
  ret->type=0;
  return ret;
}

pbodyunit *getunitbypbody(pbody *head,pbody *end)
{
  pbodyunit *rethead=createpbodyunit();
  pbody *pos=head;
  pbody *posnext;
  
  while((posnext=getpbodynext(pos))!=end){
    pbodyunit *newunit=_getunitbypbody(pos,end,&pos);
    pbodyunitappend(newunit,rethead);
  }
  return rethead;
}

//deal with a pbody, while pbodygetunit deal with the list of pbody
//newhead is the tail of pbodyunit, for example, ' a ' -> newhead== ' 
pbodyunit *_getunitbypbody(pbody *head,pbody *end,pbody **newhead)
{
  pbodyunit *ret=createpbodyunit();
  ret->type=P_NONTERMINAL;
  ret->body=head;

  pbody *pos=getpbodynext(head);
  if(pos==end) return NULL;
  int key=getpbodykey(pos);
  ret->value.index=key;
  //we assume correct input, there are 4 cases:
  //1. is '|', then this is a or operator
  //1. with '(', then we can match ')' before end
  //2. with '\'', then we can match another '\'' in the next two pbody
  //3. otherwise, just the following single pbody
  if(key=='|'){
    ret->type=P_OR;
  }
  else if(key=='('){
    pbody *poshead=pos;
    while(key!=')'){
      pos=getpbodynext(pos);
      if(pos==end) return NULL;
      key=getpbodykey(pos);
      ret->type=P_COMBINE;
    }
    pbody* postail=pos;
    ret->value.nest=getunitbypbody(poshead,postail);
  }
  else if(key=='\''){
    pos=getpbodynext(pos);
    if(pos==end) return NULL;
    int key=getpbodykey(pos);
    pos=getpbodynext(pos);
    if(pos==end) return NULL;
    ret->value.index=key;
    ret->type=P_TERMINAL;
  }
  *newhead=pos;
  return ret;
}

void printpbodyunittype(int type)
{
  switch(type){
  case P_TERMINAL:{printf("TERMINAL\t");break;}
  case P_NONTERMINAL:{printf("NONTERMINAL\t");break;}
  case P_COMBINE:{printf("P_COMBINE\t");break;}
  case P_OR:{printf("OR\t");break;}
  }
}

void _printpbodyunit(pbodyunit *u)
{
  int type=u->type;
  printpbodyunittype(type);
  if(type==P_OR){
    printf("\n");
  }
  else if(type==P_TERMINAL){
    int key=u->value.index;
    printf("%c\n",(char)key);
  }
  else if(type==P_NONTERMINAL){
    int key=u->value.index;
    printf("%d\n",key);
  }
  else if(type==P_COMBINE){
    pbodyunit *next=u->value.nest;
    printf("\n");
    printpbodyunit(next);
  }
}

void printpbodyunit(pbodyunit *listhead)
{
  pbodyunit *pos;
  for_each_pbodyunit(pos,listhead){
    _printpbodyunit(pos);
  }
}

int pcombinehasor(pbodyunit *pos)
{
  pbodyunit *nestlist=pos->value.nest;
  pbodyunit *ppos;
  for_each_pbodyunit(ppos,nestlist){
    if(ppos->type==P_OR)
      return 1;
  }
  return 0;
}

int pcombinehasalign(pbodyunit *pos)
{  
  pbodyunit *prev=pbodyunitprev(pos);
  pbodyunit *next=pbodyunitnext(pos);
  return !((pbodyunitishead(prev) && pbodyunitishead(next)));
}

void pcombineresolve(pbodyunit *pos)
{
  pbodyunit *head=pos->value.nest;
  //pbodyunit *listhead=pbodyunitprev(pos);
  pbodyunitreplace(head,pos);
  //pbodyunit *ppos;
  //for_each_pbodyunit(ppos,listhead){
  //  printf("%d ",ppos->value.index);
  //}
}

//test if a pcombine should be resolved and if true do pcombineresolve
//in a recursion way <- now I can't complete it
//true condition : ! ( hasalign && hasor )
int _pbodyunitelimateparenthese(pbodyunit *pos)
{
  int res=0;
  int hasalign=pcombinehasalign(pos);
  int hasor=pcombinehasor(pos);
  if(!(hasalign && hasor)){
    pcombineresolve(pos);
    pbodyelimateparenthese(pos->body);	
    res=1;      
  }
  return res;
}

void pbodyunitelimateparenthese(pbodyunit *listhead)
{
  pbodyunit *pos;
  int flag=1;
  while(flag){
    flag=0;
    for_each_pbodyunit(pos,listhead){
      if(pos->type==P_COMBINE){
	int tmp=_pbodyunitelimateparenthese(pos);
	if(flag==0 && tmp!=0) flag=1;
      }
    }
  }
}

//do check, check if the pbodyunit list can do elimate or
//rule : between every 'or' and begin&end, there is just one operands
int _testpbodyunitelimateor(pbodyunit *listhead)
{
  pbodyunit *iterone=pbodyunitnext(listhead);
  pbodyunit *itersec=pbodyunitnext(iterone);
  if(listhead==itersec || listhead==iterone) return 0;
  while(itersec!=listhead){
    if(itersec->type != P_OR)
      return 0;
    iterone=pbodyunitnext(itersec);
    itersec=pbodyunitnext(iterone);
  }
  return 1;
}

//compare two single pbodyunit
int pbodyunitisequal(pbodyunit *u1,pbodyunit *u2)
{
  
}
#endif
