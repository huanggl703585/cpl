#ifndef __PBODYUNIT_H
#define __PBODYUNIT_H

#include "pbody.h"

#include <stdio.h>

#define P_NONTERMINAL 1  //a symbol, like identifier, token
#define P_TERMINAL    2  //a terminal, like 'a', '|'
#define P_COMBINE     3  //a group of symbol in a pair of parentheses, liks (ab)
#define P_OR          4  // |
#define P_EMPTY       5

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
pbodyunit *createpbodyunitempty();
int pbodyunitlistisempty(pbodyunit *list);
pbodyunit *_pbodyunitcopy(pbodyunit *u);
pbodyunit *pbodyunitcopy(pbodyunit *list,pbodyunit *end);
pbodyunit *getunitbypbody(pbody *head,pbody *end);
pbodyunit *_getunitbypbody(pbody *head,pbody *end,pbody **newhead);

void printpbodyunittype(int type);
void _printpbodyunit(pbodyunit *u);
void printpbodyunit(pbodyunit *listhead);

int pcombinehasor(pbodyunit *pos);
int pcombinehasalign(pbodyunit *pos);
void pcombineresolve(pbodyunit *pos);
int _pbodyunitelimateparenthese(pbodyunit *pos);
void pbodyunitelimateparenthese(pbodyunit *listhead);

int _testpbodyunitelimateor(pbodyunit *listhead);

int _pbodyunitisequal(pbodyunit *u1,pbodyunit *u2);
int pbodyunitisequal(pbodyunit *list1,pbodyunit *list2);
int pbodyunitfindlcp(pbodyunit **unitarr,int mark[],int size,pbodyunit **head,int *headindex);

int pbodyunitfindleftrecursion(pbodyunit **unitarr,int mark[],int size,int head);

int pbodyunitlisttype(pbodyunit *list); 

#include "re_node.h"
re_node *pbodyunitbuildrenode(pbodyunit *u);
re_node *pbodyunitbuildretree(pbodyunit *list);

pbodyunit *createpbodyunit()
{
  pbodyunit *ret=(pbodyunit*)malloc(sizeof(pbodyunit));
  list_init(ret->list);
  ret->value.index=0;
  ret->type=0;
  return ret;
}

pbodyunit *createpbodyunitempty()
{
  pbodyunit *ret=createpbodyunit();
  ret->type=P_EMPTY;
  return ret;
}

int pbodyunitlistisempty(pbodyunit *list)
{
  pbodyunit *next=pbodyunitnext(list);
  return next->type==0;
}

pbodyunit *_pbodyunitcopy(pbodyunit *u)
{
  pbodyunit *ret=createpbodyunit();
  ret->type=u->type;
  ret->value.index=u->value.index;
  if(u->type==P_COMBINE)
    ret->value.nest=pbodyunitcopy(u->value.nest,u->value.nest);
      
  return ret;
}

//range:(list,end]
pbodyunit *pbodyunitcopy(pbodyunit *list,pbodyunit *end)
{
  pbodyunit *ret=createpbodyunit();
  pbodyunit *pos;
  for(pbodyunit *pos=pbodyunitnext(list);;pos=pbodyunitnext(pos)){
    pbodyunit *copyright=_pbodyunitcopy(pos);
    pbodyunitappend(copyright,ret);
    if(pos==end) break;
  }
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
  case P_EMPTY:{printf("EMPTY \t");break;}
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
//false condition : nest has no less than two member 
//               && is a operand of 'or'
int _pbodyunitelimateparenthese(pbodyunit *pos)
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
  
  pbodyunit *next=pbodyunitnext(pos);
  pbodyunit *prev=pbodyunitprev(pos);
  if(next->type==P_OR || prev->type==P_OR)
    alignor=1;
  else 
    return 0;
  
  pcombineresolve(pos);
  //pbodyelimateparenthese(pos->body);	
  return 1;
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
int _pbodyunitisequal(pbodyunit *u1,pbodyunit *u2)
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
  return pbodyunitisequal(nest1,nest2);
}

int pbodyunitisequal(pbodyunit *list1,pbodyunit *list2)
{
  for(pbodyunit *pos1=pbodyunitnext(list1),*pos2=pbodyunitnext(list2);
      pos1!=list1 || pos2!=list2;
      pos1=pbodyunitnext(pos1),pos2=pbodyunitnext(pos2)){
    if(_pbodyunitisequal(pos1,pos2)==0)
      return 0;
  }
  return 1;
}

int pbodyunitfindlcp(pbodyunit *unitarr[],int mark[],int size,pbodyunit **head,int *headindex)
{
  pbodyunit *origin[size];
  for(int i=0;i<size;i++){
    origin[i]=unitarr[i];
    unitarr[i]=pbodyunitnext(unitarr[i]);
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
	    int hascommon=_pbodyunitisequal(unitarr[i],unitarr[j]);
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
	if(!(_pbodyunitisequal(common,unitarr[k])))
	  mark[k]=0;

    //step 3 : for every list that has common pbodyunit, the list point to next pbodyunit
    for(int k=0;k<size;k++)
      if(mark[k]!=0)
	unitarr[k]=pbodyunitnext(unitarr[k]);
  }
    
  //step 4 : restore the list that don't fit the prefix
  for(int i=0;i<size;i++){
    if(mark[i]==0)
      unitarr[i]=origin[i];
    else
      unitarr[i]=pbodyunitprev(unitarr[i]); //keep the consisant
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

int pbodyunitfindleftrecursion(pbodyunit **unitarr,int mark[],int size,int head)
{
  int res=0;
  bzero(mark,sizeof(int)*size);
  for(int i=0;i<size;i++){
    pbodyunit *tmp=pbodyunitnext(unitarr[i]);
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
int pbodyunitlisttype(pbodyunit *list)
{
  pbodyunit* first=pbodyunitnext(list);
  pbodyunit* second=pbodyunitnext(first);

  if(first->type==P_EMPTY && second->type==0) return 1;
  if(first->type==P_TERMINAL && second->type==0) return 2;

  pbodyunit *pos;
  for_each_pbodyunit(pos,list){
    if(pos->type==P_COMBINE){
      pbodyunit *nest=pos->value.nest;
      int res=pbodyunitlisttype(nest);
      if(res==3) return 3;
    }
    else if(pos->type==P_NONTERMINAL)
      return 3;
  }
  return 4;
}

re_node *pbodyunitbuildrenode(pbodyunit *u)
{
  re_node *res=NULL;
  if(u->type==P_NONTERMINAL || u->type==P_TERMINAL || u->type==P_EMPTY)
    res=createrenode(RE_OPERAND,u->value.index);
  else if(u->type==P_OR)
    res=createrenode(RE_OPERATOR,RE_OR);
  else if(u->type==P_COMBINE)
    res=pbodyunitbuildretree(u->value.nest);
  return res;
}

//don't consider the condition that have recursion
re_node *pbodyunitbuildretree(pbodyunit *list)
{
  re_node *res;
  re_node *leftnode=NULL,*rightnode=NULL;
  re_node *operator=NULL;
  pbodyunit *pos=pbodyunitnext(list);
  leftnode=res=pbodyunitbuildrenode(pos);

  while((pos=pbodyunitnext(pos))!=list){
    re_node *newnode=pbodyunitbuildrenode(pos);
    //there are only 'or' operator in pbodyunit
    //so operator between pbodyunit is 'or' or 'cat'
    if(newnode->type==RE_OPERATOR){
      res=newnode;
      pos=pbodyunitnext(pos);
      rightnode=pbodyunitbuildrenode(pos);
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
#endif
