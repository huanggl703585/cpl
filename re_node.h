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
  int mark;//used to adjust dfa's output
  int index; //index in the re_node tree
  int nullable;
  set* firstpos;
  set* lastpos;
  struct re_node *left;
  struct re_node *right;
  struct re_node *parent;
};

#define isnode(node) (node->left!=NULL || node->right!=NULL)
#define isleave(node) (node->left==NULL && node->right==NULL)

re_node* createrenode(int type,int value);

re_node *createrenode(int type,int value)
{
  re_node *ret=(re_node*)malloc(sizeof(re_node));
  ret->left=ret->right=ret->parent=NULL;
  ret->firstpos=ret->lastpos=NULL;
  ret->type=type;
  ret->value=value;

  return ret;
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
