#ifndef __RE_NODE_H
#define __RE_NODE_H

#include "re.h"

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

re_node* reseqtotree(re_seq *seq)
{
  re_symbol *curoperand;
  re_node *curnode;

  int indexcnt=1;
  curoperand=list_next_entry(seq->operands,list);
  curnode=createrenode(1,curoperand);
  curnode->index=indexcnt++;

  re_operator *oper=list_next_entry(seq->operator,list);
  do{
    re_node *rightnode=NULL,*parent=NULL;
    if(oper->operator != STAR){  //operator is binary operator
      curoperand=list_next_entry(curoperand,list);
      rightnode=createrenode(1,curoperand);
      rightnode->index=indexcnt++;
    }
    parent=createrenode(0,oper);
    parent->left=curnode;
    parent->right=rightnode;
    //printf("%d %d\n",curnode->key.leave->index,parent->key.node->operator);
    curnode=parent;

  }while((oper=list_next_entry(oper,list))!=seq->operator);
  return curnode;
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
  
  re_node *rightnode;
  re_node *lastnode;
  while((curoperator=getnextoperator(iter))!=NULL){
    //printf("%d \n",curoperator->operator);
    if(curoperator->operator==RIGHTPARTH)
      return ret;
    else if(curoperator->operator==LEFTPARTH){
      ret=_buildtree(iter,index);
      lastnode=ret;
    }
    else if(curoperator->operator==STAR){
      re_node *new=createrenode(0,curoperator);
      new->left=lastnode;
      if(lastnode->parent==NULL){
	lastnode=new;
	ret=new;
      }
      else {
	lastnode->parent->right=new;
      }
      lastnode->parent=new;
    }
    else{ // CAT AND OR
      if(ret==NULL)
	ret=getchild(iter,index);
      rightnode=getchild(iter,index);
      re_node *new = createrenode(0,curoperator);
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

//post order traversal
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
