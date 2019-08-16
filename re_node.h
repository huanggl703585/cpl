#ifndef __RE_NODE_H
#define __RE_NODE_H

#include "set.h"

#include <limits.h>

#define RE_END_SYMBOL INT_MAX

#define RE_OPERATOR  1
#define RE_OPERAND   2

//for operator
#define RE_CAT  1
#define RE_OR   2
#define RE_STAR 3

typedef struct re_node re_node;
struct re_node{
  int type;
  int value; 
  int nodenum;//number of re_node of the tree
  int mark;//used to adjust dfa's output
  int index; //index in the re_node tree
  int nullable;
  set* firstpos;
  set* lastpos;
  struct re_node *left;
  struct re_node *right;
  struct re_node *parent;
};

#define isleft  1
#define isright 2

#define isnode(node) (node->left!=NULL || node->right!=NULL)
#define isleave(node) (node->left==NULL && node->right==NULL)

re_node* createrenode(int type,int value);
int retreefind(re_node *root,int value);
re_node *concatenateretree(re_node **rootarr,int size,int way);
void retreereplace(re_node *parent,int isleftchild,re_node *newtree);

int retreetestrecursion(re_node **rootarr,int head,int mark[],int size);
re_node* retreereducerecursion(re_node *root,int head,re_node *newright);

re_node *createrenode(int type,int value)
{
  re_node *ret=(re_node*)malloc(sizeof(re_node));
  ret->left=ret->right=ret->parent=NULL;
  ret->firstpos=ret->lastpos=NULL;
  ret->type=type;
  ret->value=value;
  ret->nodenum=1;

  return ret;
}

int retreefind(re_node *root,int value)
{
  if(root->type==RE_OPERAND && root->value==value)
    return 1;
  if(root->left!=NULL && root->right!=NULL)
    return (retreefind(root->left,value) || retreefind(root->right,value));
  else if(root->left!=NULL)
    return retreefind(root->left,value);
  return 0;
}

re_node *concatenateretree(re_node **rootarr,int size,int way)
{
  re_node *ret=rootarr[0];
  re_node *lefttree=ret;
  for(int i=1;i<size;i++){
    re_node *catnode=createrenode(RE_OPERATOR,way);
    catnode->left=lefttree;
    catnode->right=rootarr[i];
    catnode->nodenum+=(lefttree->nodenum+rootarr[i]->nodenum);
    lefttree=catnode;
    ret=catnode;
  }
  return ret;
}

void retreereplace(re_node *parent,int isleftchild,re_node *newtree)
{
  if(isleftchild==isleft)
    parent->left=newtree;
  else
    parent->right=newtree;
}

int retreetestrecursion(re_node **rootarr,int head,int mark[],int size)
{
  int ret=0;
  for(int i=0;i<size;i++){
    int tmp=retreefind(rootarr[i],head);
    if(tmp!=0) ret++;
    mark[i]=tmp;
  }
  return ret;
}

//just reduce one recursion symbol
re_node* retreereducerecursion(re_node *root,int head,re_node *newright)
{
  re_node *ret=root;
  if(root->right->type==RE_OPERAND && root->right->value==head){
    re_node *addstar=createrenode(RE_OPERATOR,RE_STAR);
    addstar->left=root->left;
    root->left=addstar;
    root->right=newright;
    root->nodenum++;
    return ret;
  }
  if(root->right->type==RE_OPERATOR)
    return retreereducerecursion(root->right,head,newright);
  if(root->left->type==RE_OPERATOR)
    return retreereducerecursion(root->left,head,newright);
  return NULL;
}

//=========================test/print
void printrenodetype(int type);
void printrenodeoperator(int operator);
void printrenode(re_node *node);

void printretree(re_node *tree);

void printrenodetype(int type)
{
  switch(type){
  case RE_OPERATOR:{printf("RE_OPERATOR\t");break;}
  case RE_OPERAND:{printf("RE_OPERAND\t");break;}
  }
}

void printrenodeoperator(int operator)
{
  switch(operator){
  case RE_CAT:{printf("RE_CAT");break;}
  case RE_STAR:{printf("RE_STAR");break;}
  case RE_OR:{printf("RE_OR");break;}
  }
}

void printrenode(re_node *node)
{
  printrenodetype(node->type);
  if(node->type==RE_OPERAND)
    printf("%d",node->value);
  else
    printrenodeoperator(node->value);
  printf("\n");
}

//prev order traversal
void printretree(re_node *tree)
{
  if(isnode(tree)){
    printrenode(tree);
    if(tree->left!=NULL)
      printretree(tree->left);
    if(tree->right!=NULL)
      printretree(tree->right);
  }
  else
    printrenode(tree);
}

#endif
