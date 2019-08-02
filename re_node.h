#ifndef __RE_NODE_H
#define __RE_NODE_H

#include "re_seq.h"

typedef struct re_node re_node;
struct re_node{
  union{
    re_symbol *leave;
    re_operator *node;
  }key;
  int index;
  int nullable;
  set* firstpos;
  set* lastpos;
  struct re_node *left;
  struct re_node *right;
  struct re_node *parent;
};

#define isnode(node) (node->left!=NULL || node->right!=NULL)
#define isleave(node) (node->left==NULL && node->right==NULL)

re_node* createrenode(int isleave,void *key);
re_node* reseqtotree(re_seq *seq);
re_node* buildtree(re_seq *seq);
re_node* _buildtree(re_iterator *iter,int *index);

#define ISLEAVE 1
#define ISNODE  0
re_node *createrenode(int isleave,void *key)
{
  re_node *ret=(re_node*)malloc(sizeof(re_node));
  ret->left=ret->right=ret->parent=NULL;
  if(isleave)
    ret->key.leave=(re_symbol*)key;
  else
    ret->key.node=(re_operator*)key;
  ret->firstpos=ret->lastpos=NULL;
  return ret;
}

re_node *getchild(re_iterator *iter,int *index)
{
  re_operator *operator=iter->operator;
  if(operator->operator==LEFTPARTH){
    operator=getnextoperator(iter);
    return _buildtree(iter,index);
  }
  else{
    re_symbol *operand=getnextoperands(iter);
    re_node *ret=createrenode(1,operand);
    ret->index=(*index)++;
    return ret;
  }
}

re_node* buildtree(re_seq *seq)
{
  expandreseq(seq);
  re_iterator iter;
  initreiterator(iter,seq);
  int index=1;
  return _buildtree(&iter,&index);
}
void travelretree(re_node *tree);

//TODO:
//TEST IT!
re_node* _buildtree(re_iterator *iter,int *index)
{
  re_node *ret=NULL;
  re_operator *curoperator;
  re_node *rightnode,*lastnode;

  ret=getchild(iter,index);
  lastnode=ret;

  while((curoperator=getnextoperator(iter))!=NULL){
    if(curoperator->operator==RIGHTPARTH)
      return ret;
    if(curoperator->operator==STAR){
      re_node *new=createrenode(0,curoperator);
      new->left=lastnode;
      if(lastnode->parent==NULL){
	lastnode=new;
	ret=new;
      }
      else {
	lastnode->parent->right=new;
	//lastnode unchange, it should consider duplicate star?
      }
      lastnode->parent=new;
    }
    else{ // CAT AND OR
      rightnode=getchild(iter,index);
      re_node *new = createrenode(ISLEAVE,curoperator);
      new->left=ret;
      new->right=rightnode;
      ret->parent=new;
      rightnode->parent=new;
      ret=new;
      lastnode=rightnode;
    }
  }
  return ret;
}
//=========================test/print
void travelretree(re_node *tree);
void printrenode(re_node *node);

void printrenode(re_node *node)
{
  if(isleave(node))
    printf("%d ",node->key.leave->index);
  else
    printf("%d ",node->key.node->operator);
}

//prev order traversal
void travelretree(re_node *tree)
{
  if(isnode(tree)){
    printf("%d \n",tree->key.node->operator);
    if(tree->left!=NULL)
      travelretree(tree->left);
    if(tree->right!=NULL)
      travelretree(tree->right);
  }
  else
    printf("%d \n",tree->key.leave->index);
}

#endif
