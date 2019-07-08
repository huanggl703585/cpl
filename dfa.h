#ifndef __DFA_H
#define __DFA_H

#include "set.h"
#include "re.h"

typedef struct dfa dfa;
struct dfa{
  int start;
  int end;
  re_symbol *alphabet;

};  

void createdfa(re_node *tree,int nodenum)
{
  //get a post-order travel
  //TODO: THAT NODEARR'S LENGTH SHOULD REPLACED BY A MACRO
  re_node *nodearr[256];
  nodearr[0]=tree;
  int pt=0;
  //middle--right--left
  while(pt<nodenum){
    re_node* cur=nodearr[pt];
    int temp=pt;
    if(cur->right!=NULL)
      nodearr[++temp]=cur->right;
    if(cur->left!=NULL)
      nodearr[++temp]=cur->left;
    pt++;
  }

  re_node *index[nodenum];
  set *alphabet;
  //=(set*)malloc(sizeof(set));
  //avltreeinit((*alphabet),0);
  int nodecnt=1; //number of leave+1
  //reverse -> left--right--middle
  //compute nullable, firstpos, lastpos
  for(int i=nodenum-1;i>=0;i--){
    if(isleave(nodearr[i])){
      index[nodecnt++]=nodearr[i];
      insertset((alphabet),nodearr[i]->key.leave->index);
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
  for(int i=0;i<nodenum;i++) followpos[i]=NULL;
  for(int i=nodenum-1;i>=0;i--){
    if(!isleave(nodearr[i])){
      if(nodearr[i]->key.node->operator==CAT){
	re_node *right=nodearr[i]->right;
	re_node *left=nodearr[i]->left;
	set *arr[256];
	int pt=0;
	arr[0]=left->lastpos;
	while(pt>=0){
	  set *cur=arr[pt--];
	  intersectset(&(followpos[cur->key]),right->firstpos);
	  if(cur->left!=NULL)
	    arr[++pt]=cur->left;
	  if(cur->right!=NULL)
	    arr[++pt]=cur->right;
	}
      }
      else if(nodearr[i]->key.node->operator==STAR){
	set *arr[256];
	int pt=0;
	arr[0]=nodearr[i]->lastpos;
	while(pt>=0){
	  //printf("%d ",pt);
	  set *cur=arr[pt--];
	  intersectset(&(followpos[cur->key]),nodearr[i]->firstpos);
	  if(cur->left!=NULL)
	    arr[++pt]=cur->left;
	  if(cur->right!=NULL)
	    arr[++pt]=cur->right;
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
  printf("\n");
  for(int i=1;i<nodecnt;i++){
    avltreeprint(followpos[i]);
    printf("\n");
  }
  int unmarkstate[256]={0};
  set *stateset[256];
  int spt=0;
  unmarkstate[0]=0;

}

#endif
