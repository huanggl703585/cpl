#ifndef __DFA_H
#define __DFA_H

#include "set.h"
#include "re_node.h"
#include "jumptable_link.h"
#include "dynamicarray.h"
#include "tree.h"

typedef struct dfa dfa;
struct dfa{
  int start;
  darray *end;
  int statecnt;
  set *alphabet;
  jumptable_link *jtable;
};  

typedef struct dfa_instance dfa_instance;
struct dfa_instance{
  dfa *dfa;
  int state;
  int lastendstate;
};

#define getendstate(instance)			\
  (instance->lastendstate)

#define inendstate(dfa,state)			\
  (valueindarray(dfa->end,state))

dfa* createdfa(re_node *tree,int nodenum);

dfa_instance *createdfainstance(dfa* dfa);

int walkdfa(dfa_instance *dfa,int inputstate);
void printdfa(dfa* dfa);


dfa* createdfa(re_node *tree,int nodenum)
{
  dfa *ret=(dfa*)malloc(sizeof(dfa));
  ret->jtable=createjumptablelink();
  ret->end=createdarray(1,2);

  int endposition;
  set *alphaset=NULL; //char set
  //get a post-order travel
  //TODO: THAT NODEARR'S LENGTH SHOULD REPLACED BY A MACRO
  re_node *nodearr[256];  
  nodenum=post_order_travel(tree,nodearr,256,left,right);

  printf("\n%d \n",nodenum);
  for(int i=0;i<nodenum;i++){
    printrenode(nodearr[i]);
  }
  
  //index record the leave, cooperate with nodecnt
  re_node *index[nodenum];
  int nodecnt=1; //number of leave, stands for position, so starts from 1
  //left--right--middle
  //compute nullable, firstpos, lastpos
  for(int i=0;i<nodenum;i++){
    if(isleave(nodearr[i])){
      insertset(alphaset,nodearr[i]->key.leave->index);
      //printf("p %d\n",nodearr[i]->key.leave->index);
      if(nodearr[i]->key.leave->index==0)
	endposition=nodecnt;
      index[nodecnt++]=nodearr[i];
      //printf("index %d %d\n",nodecnt-1,index[nodecnt-1]->key.leave->index);
	  
      //insertset((alphabet),nodearr[i]->key.leave->index);
      //insertset((alphaset),nodearr[i]->key.leave->index);
      if(nodearr[i]->key.leave->index==RE_EMPTY_SYMBOL)
	nodearr[i]->nullable=1;
      else{
	insertset((nodearr[i]->firstpos),nodearr[i]->index);
	insertset(nodearr[i]->lastpos,nodearr[i]->index);
      }
    }
    else{
      if(nodearr[i]->key.node->operator==OR){
	nodearr[i]->nullable=(nodearr[i]->left->nullable || nodearr[i]->right->nullable);
	intersectset(&(nodearr[i]->firstpos),nodearr[i]->left->firstpos);
	intersectset(&(nodearr[i]->firstpos),nodearr[i]->right->firstpos);
	intersectset(&(nodearr[i]->lastpos),nodearr[i]->left->lastpos);
	intersectset(&(nodearr[i]->lastpos),nodearr[i]->right->lastpos);
      }
      else if(nodearr[i]->key.node->operator==CAT){
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
  for(int i=nodenum-1;i>=0;i--){
    if(isnode(nodearr[i])){
      if(nodearr[i]->key.node->operator==CAT){
	re_node *right=nodearr[i]->right;
	re_node *left=nodearr[i]->left;
	
	set *arr[256];
	int pt=0;
	int tmpcnt=1;
	arr[0]=left->lastpos;
	while(pt>=0){
	  set *cur=arr[pt--];
	  intersectset(&(followpos[cur->key]),right->firstpos);
	  if(cur->left!=NULL)
	    arr[tmpcnt++]=cur->left;
	  if(cur->right!=NULL)
	    arr[tmpcnt++]=cur->right;
	}
      }
      else if(nodearr[i]->key.node->operator==STAR){
	set *arr[256];
	int pt=0;
	int tmpcnt=1;
	arr[0]=nodearr[i]->lastpos;
	while(pt>=0){
	  //printf("%d ",pt);
	  set *cur=arr[pt--];
	  intersectset(&(followpos[cur->key]),nodearr[i]->firstpos);
	  if(cur->left!=NULL)
	    arr[tmpcnt++]=cur->left;
	  if(cur->right!=NULL)
	    arr[tmpcnt++]=cur->right;
	}
      }
    }
  }
  
  printf("print\n");
  for(int i=nodenum-1;i>=0;i--){
    printf("%d \n",nodearr[i]->index);
    avltreeprint(nodearr[i]->firstpos);  
    avltreeprint(nodearr[i]->lastpos);
    printf("\n");
  }
  /*
  printf("\n");
  for(int i=1;i<nodecnt;i++){
    //avltreeprint(followpos[i]);
    printf("%d \n",index[i]->key.leave->index);
  }
  printf("\n");
  */
  
  int alphabet[256]={0};
  int alphacnt;
  travelavltree(alphaset,alphabet,256,alphacnt);

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
	if(alphabet[i]==(index[curposition[j]]->key.leave->index)){
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
	  mark[statecnt]=0;
	  stateset[statecnt]=tmpset;
	  hasequal=statecnt++;
	}
	//add 1 because in jumptable state index start from 1
	jumptableadd(ret->jtable,contine+1,alphabet[i],hasequal+1);
      }
    }
  }
  
  printf("\n\n");
  //NEED TO FIND END STATE
  int endstate;
  int travelarr[256]={0};
  int travelcnt;
  for(int i=0;i<statecnt;i++){
    set *travelset=stateset[i];
    bzero(travelarr,sizeof(int)*256);
    travelavltree(travelset,travelarr,256,travelcnt);
    //printf("state %d",i);
    for(int j=0;j<travelcnt;j++){
      //printf(" %d ",travelarr[j]);
      if(travelarr[j]==endposition){
	insertdarray(ret->end,(void*)(j+1));
      }
    }
    printf("\n");
  }
  
  ret->start=1;
  ret->alphabet=alphaset;
  ret->statecnt=statecnt;
  printdfa(ret);
  return ret;
}

void printdfa(dfa* dfa)
{
  printf("dfa start %d\n",dfa->start);
  printf("dfa end ");
  printdarray(dfa->end);
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
  if(valueindarray(instance->dfa->end,instance->state,intequal)){
    //instance->state=instance->dfa->start;
    instance->lastendstate=ret;
  }
  instance->state=ret;
  return ret;
}

#endif
