#ifndef __AST_H
#define __AST_H

#include "symboltable.h"

typedef struct ast ast;
typedef struct astnode astnode;

struct ast{
  symboltable *table;
  astnode *root;
};

struct astnode{
  int index;
  int childcnt;
  astnode **child;
};

ast *createast(symboltable *table);
astnode *createastnode(int index);
void initast(ast *tree,int start);
void expandast(astnode *node,pbody *body);

ast *createast(symboltable *table)
{
  ast *ret=(ast*)malloc(sizeof(ast));
  ret->table=table;
  ret->root=NULL;
  return NULL;
}

astnode *createastnode(int index)
{
  astnode *ret=(astnode*)malloc(sizeof(astnode));
  ret->index=index;
  ret->childcnt=0;
  ret->child=NULL;
  return ret;
}

void initast(ast *tree,int start)
{
  astnode *root=createastnode(start);
  tree->root=root;
}

void expandast(astnode *node,pbody *body)
{
  pbody *pos;
  int cnt=0;
  pbody_for_each(pos,body){
    cnt++;
  }
  node->child=(astnode**)malloc(sizeof(astnode*)*cnt);
  int pt=0;
  pbody_for_each(pos,body){
    int key=getpbodykey(pos);
    astnode *tmp=createastnode(key);
    node->child[pt++]=tmp;
  }
}

#endif
