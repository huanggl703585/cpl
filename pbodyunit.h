#ifndef __PBODYUNIT_H
#define __PBODYUNIT_H

#include "pbody.h"

#include <stdio.h>

#define P_NONTERMINAL 1  //a symbol, like identifier, token
#define P_TERMINAL    2  //a terminal, like 'a', '|'
#define P_COMBINE     3  //a group of symbol in a pair of parentheses, liks (ab)
#define P_OR          4  // |
#define P_EMPTY       5

//used for structual grammar
#define P_LEFT        6
#define P_RIGHT       7

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

#define pbodyunitAppend(newlist,listhead)			\
  listaddtail(&(newlist->list),&(listhead->list))

#define pbodyunitAdd(new,pos)			\
  listadd(&(new->list),&(pos->list))

#define pbodyunitInsert(newlist,pos)		\
  listadd(&(newlist->list),&(pos->list))

#define pbodyunitDrop(pos)			\
  listdrop(&(pos->list))

#define pbodyunitPrev(pos)			\
  list_prev_entry(pos,list)			

#define pbodyunitNext(pos)			\
  list_next_entry(pos,list)

#define pbodyunitIsHead(pos)			\
  (pos->type==0)

#define pbodyunitReplace(newhead,replacedpos)	\
  listreplace(&(newhead->list),&(replacedpos->list))

pbodyunit *createPbodyunit();
pbodyunit *createPbodyunitEmpty();
int pbodyunitListIsEmpty(pbodyunit *list);
pbodyunit *_pbodyunitCopy(pbodyunit *u);
pbodyunit *pbodyunitCopy(pbodyunit *list,pbodyunit *end);
pbodyunit *getUnitByPbody(pbody *head,pbody *end);
pbodyunit *_getUnitByPbody(pbody *head,pbody *end,pbody **newhead);

void printPbodyunitType(int type);
void _printPbodyunit(pbodyunit *u);
void printPbodyunit(pbodyunit *listhead);

int pcombineHasOr(pbodyunit *pos);
int pcombineHasAlign(pbodyunit *pos);
void pcombineResolve(pbodyunit *pos);
int _pbodyunitElimateParenthese(pbodyunit *pos);
void pbodyunitElimateParenthese(pbodyunit *listhead);

int _testPbodyunitElimateOr(pbodyunit *listhead);

int _pbodyunitIsEqual(pbodyunit *u1,pbodyunit *u2);
int pbodyunitIsEqual(pbodyunit *list1,pbodyunit *list2);
int pbodyunitFindLcp(pbodyunit **unitarr,int mark[],int size,pbodyunit **head,int *headindex);

int pbodyunitFindLeftRecursion(pbodyunit **unitarr,int mark[],int size,int head);

int pbodyunitListType(pbodyunit *list); 

#include "re_node.h"
re_node *pbodyunitBuildRenode(pbodyunit *u);
re_node *pbodyunitBuildRetree(pbodyunit *list);

void pbodyunitAddParenthese(pbodyunit *list);
void elimatePbodyunitNest(pbodyunit *list);

pbodyunit *createPbodyunit()
{
  pbodyunit *ret=(pbodyunit*)malloc(sizeof(pbodyunit));
  list_init(ret->list);
  ret->value.index=0;
  ret->type=0;
  return ret;
}

pbodyunit *createPbodyunitEmpty()
{
  pbodyunit *ret=createPbodyunit();
  ret->type=P_EMPTY;
  return ret;
}

int pbodyunitListIsEmpty(pbodyunit *list)
{
  pbodyunit *next=pbodyunitNext(list);
  return next->type==0;
}

pbodyunit *_pbodyunitCopy(pbodyunit *u)
{
  pbodyunit *ret=createPbodyunit();
  ret->type=u->type;
  ret->value.index=u->value.index;
  if(u->type==P_COMBINE)
    ret->value.nest=pbodyunitCopy(u->value.nest,u->value.nest);
      
  return ret;
}

//range:(list,end]
pbodyunit *pbodyunitCopy(pbodyunit *list,pbodyunit *end)
{
  pbodyunit *ret=createPbodyunit();
  pbodyunit *pos;
  for(pbodyunit *pos=pbodyunitNext(list);;pos=pbodyunitNext(pos)){
    pbodyunit *copyright=_pbodyunitCopy(pos);
    pbodyunitAppend(copyright,ret);
    if(pos==end) break;
  }
  return ret;
}

pbodyunit *getUnitByPbody(pbody *head,pbody *end)
{
  pbodyunit *rethead=createPbodyunit();
  pbody *pos=head;
  pbody *posnext;
  
  while((posnext=getPbodyNext(pos))!=end){
    pbodyunit *newunit=_getUnitByPbody(pos,end,&pos);
    pbodyunitAppend(newunit,rethead);
  }
  return rethead;
}

//deal with a pbody, while pbodygetunit deal with the list of pbody
//newhead is the tail of pbodyunit, for example, ' a ' -> newhead== ' 
pbodyunit *_getUnitByPbody(pbody *head,pbody *end,pbody **newhead)
{
  pbodyunit *ret=createPbodyunit();
  ret->type=P_NONTERMINAL;
  ret->body=head;

  pbody *pos=getPbodyNext(head);
  if(pos==end) return NULL;
  int key=getPbodyKey(pos);
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
      pos=getPbodyNext(pos);
      if(pos==end) return NULL;
      key=getPbodyKey(pos);
      ret->type=P_COMBINE;
    }
    pbody* postail=pos;
    ret->value.nest=getUnitByPbody(poshead,postail);
  }
  else if(key=='\''){
    pos=getPbodyNext(pos);
    if(pos==end) return NULL;
    int key=getPbodyKey(pos);
    pos=getPbodyNext(pos);
    if(pos==end) return NULL;
    ret->value.index=key;
    ret->type=P_TERMINAL;
  }
  *newhead=pos;
  return ret;
}

void printPbodyunitType(int type)
{
  switch(type){
  case P_TERMINAL:{printf("TERMINAL\t");break;}
  case P_NONTERMINAL:{printf("NONTERMINAL\t");break;}
  case P_COMBINE:{printf("P_COMBINE\t");break;}
  case P_OR:{printf("OR\t");break;}
  case P_EMPTY:{printf("EMPTY\t");break;}
  case P_LEFT: {printf("LEFTPARENTHESE\t");break;}
  case P_RIGHT:{printf("RIGHTPARENTHESE\t");break;}
  }
}

void _printPbodyunit(pbodyunit *u)
{
  int type=u->type;
  printPbodyunitType(type);
  if(type==P_OR || type==P_EMPTY){
    printf("\n");
  }
  else if(type==P_TERMINAL){
    int key=u->value.index;
    //printf("%d\n",(char)key);
    printf("%d\n",key);
  }
  else if(type==P_NONTERMINAL){
    int key=u->value.index;
    printf("%d\n",key);
  }
  else if(type==P_COMBINE){
    pbodyunit *next=u->value.nest;
    printf("\n");
    printPbodyunit(next);
  }
}

void printPbodyunit(pbodyunit *listhead)
{
  pbodyunit *pos;
  for_each_pbodyunit(pos,listhead){
    _printPbodyunit(pos);
  }
}

int pcombineHasOr(pbodyunit *pos)
{
  pbodyunit *nestlist=pos->value.nest;
  pbodyunit *ppos;
  for_each_pbodyunit(ppos,nestlist){
    if(ppos->type==P_OR)
      return 1;
  }
  return 0;
}

int pcombineHasAlign(pbodyunit *pos)
{  
  pbodyunit *prev=pbodyunitPrev(pos);
  pbodyunit *next=pbodyunitNext(pos);
  return !((pbodyunitIsHead(prev) && pbodyunitIsHead(next)));
}

void pcombineResolve(pbodyunit *pos)
{
  pbodyunit *head=pos->value.nest;
  //pbodyunit *listhead=pbodyunitPrev(pos);
  pbodyunitReplace(head,pos);
  //pbodyunit *ppos;
  //for_each_pbodyunit(ppos,listhead){
  //  printf("%d ",ppos->value.index);
  //}
}

//test if a pcombine should be resolved and if true do pcombineResolve
//in a recursion way <- now I can't complete it
//false condition : nest has no less than two member 
//               && is a operand of 'or'
int _pbodyunitElimateParenthese(pbodyunit *pos)
{
  int res=0;
  int alignor=0;
  int enoughmember=0;

  int membercnt=0;
  pbodyunit *nest=pos->value.nest;
  pbodyunit *ppos;
  for_each_pbodyunit(ppos,nest){
    if(++membercnt == 2){
      enoughmember=1;break;
    }
  }
  if(enoughmember==0) return 0;
  
  pbodyunit *next=pbodyunitNext(pos);
  pbodyunit *prev=pbodyunitPrev(pos);
  if(next->type==P_OR || prev->type==P_OR)
    alignor=1;
  else 
    return 0;
  
  pcombineResolve(pos);
  //pbodyelimateparenthese(pos->body);	
  return 1;
}

void pbodyunitElimateParenthese(pbodyunit *listhead)
{
  pbodyunit *pos;
  int flag=1;
  while(flag){
    flag=0;
    for_each_pbodyunit(pos,listhead){
      if(pos->type==P_COMBINE){
	int tmp=_pbodyunitElimateParenthese(pos);
	if(flag==0 && tmp!=0) flag=1;
      }
    }
  }
}

//do check, check if the pbodyunit list can do elimate or
//rule : between every 'or' and begin&end, there is just one operands
int _testPbodyunitElimateOr(pbodyunit *listhead)
{
  pbodyunit *iterone=pbodyunitNext(listhead);
  pbodyunit *itersec=pbodyunitNext(iterone);
  if(listhead==itersec || listhead==iterone) return 0;
  while(itersec!=listhead){
    if(itersec->type != P_OR)
      return 0;
    iterone=pbodyunitNext(itersec);
    itersec=pbodyunitNext(iterone);
  }
  return 1;
}

//compare two single pbodyunit
int _pbodyunitIsEqual(pbodyunit *u1,pbodyunit *u2)
{
  int type1=u1->type,type2=u2->type;
  if(u1->type != u2->type) return 0;

  if(u1->type==P_OR || u1->type==P_EMPTY) return 1;
  else if(u1->type==P_NONTERMINAL)
    return (u1->value.index==u2->value.index);
  else if(u1->type==P_TERMINAL)
    return (u1->value.index==u2->value.index) && u1->value.index!=0; //avoid empty
  //else u1->type==P_COMBIE
  pbodyunit *nest1=u1->value.nest;
  pbodyunit *nest2=u2->value.nest;
  return pbodyunitIsEqual(nest1,nest2);
}

int pbodyunitIsEqual(pbodyunit *list1,pbodyunit *list2)
{
  for(pbodyunit *pos1=pbodyunitNext(list1),*pos2=pbodyunitNext(list2);
      pos1!=list1 || pos2!=list2;
      pos1=pbodyunitNext(pos1),pos2=pbodyunitNext(pos2)){
    if(_pbodyunitIsEqual(pos1,pos2)==0)
      return 0;
  }
  return 1;
}

int pbodyunitFindLcp(pbodyunit *unitarr[],int mark[],int size,pbodyunit **head,int *headindex)
{
  pbodyunit *origin[size];
  for(int i=0;i<size;i++){
    origin[i]=unitarr[i];
    unitarr[i]=pbodyunitNext(unitarr[i]);
    mark[i]=1;
  }
  int lcpisempty=1;
  while(1){
    //step1 : find a "seed", here we find a common pbodyunit between two list
    pbodyunit *common=NULL;
    int i,j;
    for(i=0;i<size-1;i++){
      if(mark[i]!=0){
	for(j=i+1;j<size;j++){
	  if(mark[j]!=0){
	    int hascommon=_pbodyunitIsEqual(unitarr[i],unitarr[j]);
	    if(hascommon){
	      common=unitarr[i];
	      if(lcpisempty==1) lcpisempty=0;
	      goto findeverycommon;
	    }
	  }
	}
      }
    }
    //if can't find the common between every two list, then have not any longer common prefix
    if(common==NULL) break;

    //step 2 : find every list that has common pbodyunit, if a list has not, then its mark==0
  findeverycommon:
    for(int k=0;k<size;k++)
      if(k!=i || k!=j)
	if(!(_pbodyunitIsEqual(common,unitarr[k])))
	  mark[k]=0;

    //step 3 : for every list that has common pbodyunit, the list point to next pbodyunit
    for(int k=0;k<size;k++)
      if(mark[k]!=0)
	unitarr[k]=pbodyunitNext(unitarr[k]);
  }
    
  //step 4 : restore the list that don't fit the prefix
  for(int i=0;i<size;i++){
    if(mark[i]==0)
      unitarr[i]=origin[i];
    else
      unitarr[i]=pbodyunitPrev(unitarr[i]); //keep the consisant
  }

  if(lcpisempty==1) return 0;
  for(int i=0;i<size;i++)
    if(mark[i]!=0){
      *head=origin[i];
      *headindex=i;
      return 1;
    }
  return 0;
}

int pbodyunitFindLeftRecursion(pbodyunit **unitarr,int mark[],int size,int head)
{
  int res=0;
  bzero(mark,sizeof(int)*size);
  for(int i=0;i<size;i++){
    pbodyunit *tmp=pbodyunitNext(unitarr[i]);
    if(tmp->type==P_NONTERMINAL && tmp->value.index==head){
      mark[i]=1;
      res=1;
    }
  }
  return res;
}

/*
  return value:
  1 : map to S_HAVEEMPTY
  2 : that pbodyunit has a single terminal
  3 : that pbodyunit has at least a nonterminal
  4 : other-map to S_TERMINALSET
*/
int pbodyunitListType(pbodyunit *list)
{
  pbodyunit* first=pbodyunitNext(list);
  pbodyunit* second=pbodyunitNext(first);

  if(first->type==P_EMPTY && second->type==0) return 1;
  if(first->type==P_TERMINAL && second->type==0) return 2;

  pbodyunit *pos;
  for_each_pbodyunit(pos,list){
    if(pos->type==P_COMBINE){
      pbodyunit *nest=pos->value.nest;
      int res=pbodyunitListType(nest);
      if(res==3) return 3;
    }
    else if(pos->type==P_NONTERMINAL)
      return 3;
  }
  return 4;
}

re_node *pbodyunitBuildRenode(pbodyunit *u)
{
  re_node *res=NULL;
  if(u->type==P_NONTERMINAL || u->type==P_TERMINAL || u->type==P_EMPTY)
    res=createrenode(RE_OPERAND,u->value.index);
  else if(u->type==P_OR)
    res=createrenode(RE_OPERATOR,RE_OR);
  else if(u->type==P_COMBINE)
    res=pbodyunitBuildRetree(u->value.nest);
  return res;
}

//don't consider the condition that have recursion
re_node *pbodyunitBuildRetree(pbodyunit *list)
{
  re_node *res;
  re_node *leftnode=NULL,*rightnode=NULL;
  re_node *operator=NULL;
  pbodyunit *pos=pbodyunitNext(list);
  leftnode=res=pbodyunitBuildRenode(pos);

  while((pos=pbodyunitNext(pos))!=list){
    re_node *newnode=pbodyunitBuildRenode(pos);
    //there are only 'or' operator in pbodyunit
    //so operator between pbodyunit is 'or' or 'cat'
    if(newnode->type==RE_OPERATOR){
      res=newnode;
      pos=pbodyunitNext(pos);
      rightnode=pbodyunitBuildRenode(pos);
      newnode->left=leftnode;
      newnode->right=rightnode;
      newnode->nodenum+=(leftnode->nodenum+rightnode->nodenum);
      leftnode=newnode;
    }
    else{
      re_node *catnode=createrenode(RE_OPERATOR,RE_CAT);
      res=catnode;
      catnode->left=leftnode;
      catnode->right=newnode;
      catnode->nodenum+=(leftnode->nodenum+newnode->nodenum);
      leftnode=catnode;
    }
  }
  return res;
}

void pbodyunitAddParenthese(pbodyunit *list)
{
  pbodyunit *left=createPbodyunit(),*right=createPbodyunit();
  left->type=P_LEFT;
  right->type=P_RIGHT;
  pbodyunitAdd(left,list);
  pbodyunitAppend(right,list);
}

void elimatePbodyunitNest(pbodyunit *list)
{
  pbodyunit *tmp;
  for(pbodyunit *pos=pbodyunitNext(list),*tmp=pbodyunitNext(pos);
      pos!=list;
      pos=tmp,tmp=pbodyunitNext(tmp)){
    if(pos->type==P_COMBINE){
      pbodyunit *nest=pos->value.nest;
      elimatePbodyunitNest(nest);
      pbodyunitAddParenthese(nest);
      pbodyunitReplace(nest,pos);
    }
  }
}

#endif
