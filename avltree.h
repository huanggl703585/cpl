#ifndef __AVLTREE_H
#define __AVLTREE_H

#include <stdlib.h>
#include "utli.h"

struct avltree{
  int key;
  int factor;          //new node's factor==0
  struct avltree *parent;
  struct avltree *left;
  struct avltree *right;
};

#define avltreeinit(node,keynum)		\
  node.key=keynum;node.factor=0;			\
  node.parent=node.left=node.right=NULL

//recver is a array
#define travelavltree(root,recver,maxnum,cnt)		\
  do{							\
    int __cnt=0;					\
    if(root==NULL)					\
      ;							\
    else{						\
      struct avltree *__arr[maxnum];				\
      __arr[0]=root;						\
      int __pt=0;						\
      while(__pt>=0){						\
	struct avltree *__cur=__arr[__pt--];			\
	if(++__cnt==maxnum)					\
	  break;						\
	arraysortinsert(recver,maxnum,__cur->key);		\
	if(__cur->left!=NULL)					\
	  __arr[++__pt]=__cur->left;					\
	if(__cur->right!=NULL)						\
	  __arr[++__pt]=__cur->right;					\
      }									\
    }									\
    cnt=__cnt;								\
  }while(0)							

void avltreeinsert(struct avltree** root,int key);
void _avltreeinsert(struct avltree** root,struct avltree* new);
void avltreedelete(struct avltree* root,int key);
int avltreefind(struct avltree* root,int key);
struct avltree* _avltreefind(struct avltree* root,int key);
void avltreealter(struct avltree* root,int oldkey,int newkey);
void avltreeprint(struct avltree* root);

void avlrightrotate(struct avltree **root,struct avltree* point,int height);
void avlleftrotate(struct avltree **root,struct avltree* point,int height);

void rebalanceavltree(struct avltree** root,struct avltree* parent,int isleft);

void avltreeinsert(struct avltree** root,int key)
{
  struct avltree *new=(struct avltree*)malloc(sizeof(struct avltree));
  avltreeinit((*new),key);
  _avltreeinsert(root,new);
}

void _avltreeinsert(struct avltree **root,struct avltree *new)
{
  if(*root==NULL){
    *root=new;
    return ;
  }
  struct avltree *pt=*root;
  int isleft=0;
  while(1){
    struct avltree *next;
    if(pt->key==new->key)
      return ;
    else if(pt->key<new->key){
      pt->factor++;
      if(pt->right!=NULL)
	pt=pt->right;
      else{
	pt->right=new;
	new->parent=pt;
	break;
      }
    }
    else{
      pt->factor--;
      if(pt->left!=NULL)
	pt=pt->left;
      else{
	pt->left=new;
	new->parent=pt;
	isleft=1;
	break;
      }
    }
  }
  //rebalanceavltree(root,pt,isleft);
}

void rebalanceavltree(struct avltree** root,struct avltree* parent,int isleft)
{
  int parentisleft=-1;
  int height=1;
  while(1){    
    if(parent->parent!=NULL && parent->parent->left==parent)
      parentisleft=1;
    else if(parent->parent!=NULL && parent->parent->right==parent)
      parentisleft=0;

    if(parentisleft!=-1 && parentisleft ^ isleft){
      if(parentisleft)
	avlleftrotate(root,parent,height);
      else 
	avlrightrotate(root,parent,height);
      parent=parent->parent;
    }
    else{
      if(parent->factor==-2){
	avlrightrotate(root,parent,height);
	parent=parent->parent;
	struct avltree* temp=parent;
	while(temp->parent!=NULL){
	  temp->parent->factor++;
	  temp=temp->parent;
	}
      }
      else if(parent->factor==2){
	avlleftrotate(root,parent,height);
	parent=parent->parent;
	struct avltree* temp=parent;
	while(temp->parent!=NULL){
	  temp->parent->factor--;
	  temp=temp->parent;
	}
      }
    }

    isleft=parentisleft;
    parent=parent->parent;
    height++;
    if(parent==NULL)
      break;
  }
}

void avlrightrotate(struct avltree** root,struct avltree* point,int height)
{
  struct avltree* left=point->left;
  int ha=(left->factor==1)?(height-2):(height-1);
  int hb=(left->factor==-1)?(height-2):(height-1);
  int hc=(height+point->factor);
  int hpt=(hb==0 && hc==0)?1:(max(hb,hc)+1);
  point->factor=hc-hb;
  left->factor=hpt-ha;

  left->parent=point->parent;
  point->left=left->right;
  if(left->right!=NULL)
    left->right->parent=point;

  if(point->parent==NULL)
    *root=left;
  else if(point==point->parent->left)
    point->parent->left=left;
  else
    point->parent->right=left;

  left->right=point;
  point->parent=left;
}

void avlleftrotate(struct avltree** root,struct avltree* point,int height)
{
  struct avltree* right=point->right;
  int ha=((right->factor==1)?(height-2):(height-1));
  int hb=((right->factor==-1)?(height-2):(height-1));
  int hc=(height-point->factor);
  int hpt=(ha==0 && hc==0)?1:(max(ha,hc)+1);
  point->factor=hc-ha;
  right->factor=hb-hpt;
  //printf("%d %d %d %d %d %d\n",ha,hb,hc,hpt,point->factor,right->factor);
  if(point->parent==NULL)
    *root=right;
  else if(point==point->parent->left)
    point->parent->left=right;
  else
    point->parent->right=right;

  right->parent=point->parent;
  point->parent=right;
  point->right=right->left;
  if(right->left!=NULL)
    right->left->parent=point;

  right->left=point;
}

void avltreeprint(struct avltree* root)
{
  if(root==NULL){
    printf("empty avltree/set\n");
    return;
  }

  //TODO: USE A MACRO TO REPLACED THE LENGTH OF ARRAY
  struct avltree *arr[256];
  arr[0]=root;
  int pt=0;
  
  while(pt>=0){
    struct avltree *cur=arr[pt];
    pt--;
    int left=(cur->left!=NULL)?cur->left->key:-1;
    int right=(cur->right!=NULL)?cur->right->key:-1;
    //printf("%p key %d factor %d left %d right %d parent %p\n",cur,cur->key,cur->factor,left,right,cur->parent);
    printf("%d ",cur->key);
    if(cur->left!=NULL)
      arr[++pt]=cur->left;
    if(cur->right!=NULL)
      arr[++pt]=cur->right;
  }
  printf("\n");
}

int avltreefind(struct avltree* root,int key){
  if(root==NULL) return 0;
  return (_avltreefind(root,key)!=NULL);
}

struct avltree* _avltreefind(struct avltree* root,int key)
{
  struct avltree *pt=root;
  while(pt!=NULL){
    if(pt->key==key)
      return pt;
    else if(pt->key<key)
      pt=pt->right;
    else
      pt=pt->left;
  }
  return NULL;
}

//NO REPEAT ELEMENT!!
int avltreecmp(struct avltree* t1,struct avltree *t2)
{
  int cnt1=0,cnt2=0;
  int arr1[256],arr2[256];
  bzero(arr1,sizeof(int)*256);
  bzero(arr2,sizeof(int)*256);

  travelavltree(t1,arr1,256,cnt1);
  travelavltree(t2,arr2,256,cnt2);
  
  //printf("%d %d\n",cnt1,cnt2);
  //for(int j=0;j<cnt1;j++)
  //  printf("%d %d\n",arr1[j],arr2[j]);
  return intarrayisequal(arr1,arr2,cnt1,cnt2);
}

#endif
