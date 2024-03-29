#ifndef __DFA_H
#define __DFA_H

#include "set.h"
#include "re_node.h"
#include "jumptable_link.h"
#include "kvpair.h"
#include "tree.h"

typedef kvpair endstate;

int findendstate(endstate* list,int state)
{
  return intkvpairfind(list,state);
}

typedef struct dfa dfa;
struct dfa{
  int start;
  //darray *end;
  //key: end state; value: output
  endstate *end;
  int statecnt;
  set *alphabet;
  jumptable_link *jtable;
};  

typedef struct dfa_instance dfa_instance;
struct dfa_instance{
  dfa *dfa;
  int state;
  int lastend;
};

#define initendstate()				\
  (endstate*)_createkvpair(NULL,NULL)		

#define endstateappend(head,stateid,output)	\
  kvpairappend(head,stateid,output)

#define restartdfainstance(instance)		\
  instance->state=instance->dfa->start;		\
  instance->lastend=0

#define dfainstancecopy(origin,copyright)	\
  memcpy(copyright,origin,sizeof(dfa_instance))

#define dfainstanceundo(copyright,origin)	\
  dfainstancecopy(copyright,origin)

#define dfainstartstate(instance)		\
  (instance->state==instance->dfa->start &&	\
   instance->lastend==0)

//state's type is set*
#define getstatemark(state,leavearr) ({			\
      int key=state->key;				\
      re_node *node=leavearr[key];			\
      node->mark;})

dfa* createdfa(re_node *tree,int nodenum);

dfa_instance *createdfainstance(dfa* dfa);

int walkdfa(dfa_instance *dfa,int inputstate);
void printdfa(dfa* dfa);

dfa* createdfa(re_node *tree,int nodenum)
{
  dfa *ret=(dfa*)malloc(sizeof(dfa));
  ret->jtable=createjumptablelink();
  ret->end=initendstate();

  int endposition;
  set *alphaset=NULL; //char set
  //get a post-order travel
  //TODO: THAT NODEARR'S LENGTH SHOULD REPLACED BY A MACRO
  re_node *nodearr[256];  
  int nodecnt=post_order_travel(tree,nodearr,256,left,right);
  
  //index record the leave, cooperate with nodecnt
  re_node *leave[nodecnt];
  int leavecnt=1; //number of leave, stands for position, so starts from 1
  //left--right--middle
  //compute nullable, firstpos, lastpos
  for(int i=0;i<nodenum;i++){
    if(isleave(nodearr[i])){
      insertset(alphaset,nodearr[i]->value);
      nodearr[i]->index=leavecnt;
      leave[leavecnt++]=nodearr[i];

      if(nodearr[i]->value==RE_END_SYMBOL)
	endposition=nodearr[i]->index;
   
      if(nodearr[i]->value==0)
	nodearr[i]->nullable=1;
      else{
	insertset(nodearr[i]->firstpos,nodearr[i]->index);
	insertset(nodearr[i]->lastpos,nodearr[i]->index);
      }
    }
    else{
      if(nodearr[i]->value==RE_OR){
	nodearr[i]->nullable=(nodearr[i]->left->nullable || nodearr[i]->right->nullable);
	intersectset(&(nodearr[i]->firstpos),nodearr[i]->left->firstpos);
	intersectset(&(nodearr[i]->firstpos),nodearr[i]->right->firstpos);
	intersectset(&(nodearr[i]->lastpos),nodearr[i]->left->lastpos);
	intersectset(&(nodearr[i]->lastpos),nodearr[i]->right->lastpos);
      }
      else if(nodearr[i]->value==RE_CAT){
	nodearr[i]->nullable=(nodearr[i]->left->nullable && nodearr[i]->right->nullable);
	if(nodearr[i]->left->nullable==1){
	  intersectset(&(nodearr[i]->firstpos),nodearr[i]->left->firstpos);
	  intersectset(&(nodearr[i]->firstpos),nodearr[i]->right->firstpos);
	}
	else
	  intersectset(&(nodearr[i]->firstpos),nodearr[i]->left->firstpos);
	if(nodearr[i]->right->nullable==1){
	  intersectset(&(nodearr[i]->lastpos),nodearr[i]->left->lastpos);
	  intersectset(&(nodearr[i]->lastpos),nodearr[i]->right->lastpos);
	}
	else
	  intersectset(&(nodearr[i]->lastpos),nodearr[i]->right->firstpos);
      }
      else{ //operator==STAR
	nodearr[i]->nullable=1;
	intersectset(&(nodearr[i]->firstpos),nodearr[i]->left->firstpos);
	intersectset(&(nodearr[i]->lastpos),nodearr[i]->left->lastpos);
      }
    }
  }

  //compute followpos
  //nodecnt is number of leave
  set *followpos[nodecnt];
  for(int i=0;i<nodecnt;i++)
    followpos[i]=NULL;
  for(int i=0;i<nodecnt;i++){
    if(isnode(nodearr[i])){
      if(nodearr[i]->value==RE_CAT){
	re_node *right=nodearr[i]->right;
	re_node *left=nodearr[i]->left;
	
	int arr[leavecnt];
	int num;
	travelavltree(left->lastpos,arr,leavecnt,num);
	//avltreeprint(right->firstpos);
	for(int i=0;i<num;i++){
	  //printf("index %d\n",arr[i]);
	  intersectset(&(followpos[arr[i]]),right->firstpos);
	}
      }
      else if(nodearr[i]->value==RE_STAR){
	int arr[leavecnt];
	int num;
	re_node *cur=nodearr[i];
	travelavltree(cur->firstpos,arr,leavecnt,num);
	//avltreeprint(cur->firstpos);
	for(int i=0;i<num;i++){
	  //printf("index %d\n",arr[i]);
	  intersectset(&(followpos[arr[i]]),cur->firstpos);
	}
      }
    }
  }

  /*
  printf("print\n");
  for(int i=0;i<nodecnt;i++){
    printf("%d \n",nodearr[i]->index);
    avltreeprint(nodearr[i]->firstpos);  
    avltreeprint(nodearr[i]->lastpos);
    printf("\n");
  }
  
  printf("\n");
  for(int i=1;i<leavecnt;i++){
    printf("%d ",i);
    avltreeprint(followpos[i]);
  }
  printf("\n");
  */
  
  int alphabet[256]={0};
  int alphacnt;
  travelavltree(alphaset,alphabet,256,alphacnt);

  int markarray[256]={0};
  
  int statecnt=0;
  int mark[256]={0};
  set *stateset[256]={NULL};
  int tmp[256]={0};
  int tmpcnt=0;
  mark[0]=0;
  //init first state
  set* curstate=NULL;
  bzero(tmp,sizeof(int)*256);
  set* tmpset=tree->firstpos;
  travelavltree((tree->firstpos),tmp,256,tmpcnt);
  for(int i=0;i<tmpcnt;i++)
    insertset(curstate,tmp[i]);
  markarray[statecnt]=getstatemark(curstate,leave);
  stateset[statecnt++]=curstate;
  
  while(1){
    int contine=arrayhaszero(mark,statecnt,intiszero);
    //printf("contine %d ",contine);
    if(contine==-1) break;
    curstate=stateset[contine];
    int curposition[256]={0};
    int curcnt;
    travelavltree(curstate,curposition,256,curcnt);
    
    mark[contine]=1;
    for(int i=0;i<alphacnt;i++){
      set *tmpset=NULL;
      for(int j=0;j<curcnt;j++){
	//printf("\nprintfollowpos\n");
	//avltreeprint(followpos[curposition[j]]);
	if(alphabet[i]==(leave[curposition[j]]->value)){
	  intersectset(&tmpset,(followpos[curposition[j]]));
	}
      }
      if(tmpset!=NULL){
	//printf("\nnewstate\n");
	//avltreeprint(tmpset);
	//printf("\n");
	int hasequal=arrayfindequal(stateset,statecnt,tmpset,avltreecmp);
	//printf("hasequal %d\n",hasequal);
	if(hasequal==-1){ //add state
	  if((markarray[statecnt]=getstatemark(tmpset,leave))==0)
	    markarray[statecnt]=markarray[contine];
	  mark[statecnt]=0;
	  stateset[statecnt]=tmpset;
	  hasequal=statecnt++;
	}
	//add 1 because in jumptable state index start from 1
	jumptableadd(ret->jtable,contine+1,alphabet[i],hasequal+1);
      }
    }
  }
  //printf("\n\n");
  
  //find end state
  int travelarr[256]={0};
  int travelcnt;
  for(int i=0;i<statecnt;i++){
    set *travelset=stateset[i];
    bzero(travelarr,sizeof(int)*256);
    travelavltree(travelset,travelarr,256,travelcnt);
    //printf("state %d",i);
    for(int j=0;j<travelcnt;j++){
      //printf(" %d ",travelarr[j]);
      if(leave[travelarr[j]]->value==RE_END_SYMBOL &&
	 leave[travelarr[j]]->mark!=0)
	endstateappend((ret->end),i+1,leave[travelarr[j]]->mark);
    }
    //printf("\n");
  }

  ret->start=1;
  ret->alphabet=alphaset;
  ret->statecnt=statecnt;
  // printdfa(ret);
  return ret;
}



void printdfa(dfa* dfa)
{
  printf("dfa start %d\n",dfa->start);
  printf("dfa end\n");
  printintkvpair(dfa->end);
  printjumptable(dfa->jtable);
}

dfa_instance * createdfainstance(dfa *dfa)
{
  dfa_instance *ret=(dfa_instance*)malloc(sizeof(dfa_instance));
  ret->dfa=dfa;
  ret->state=dfa->start;
}

int walkdfa(dfa_instance *instance,int input)
{
  int ret=jumptablefind(instance->dfa->jtable,instance->state,input);
  int isend;
  if((isend=findendstate(instance->dfa->end,instance->state))!=0)
    instance->lastend=isend;
  instance->state=ret;
  return ret;
}

#endif
