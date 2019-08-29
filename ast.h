#ifndef __AST_H
#define __AST_H

#include "tokenlist.h"
#include "symboltable.h"

typedef struct ast ast;
typedef struct astnode astnode;

struct ast{
  symboltable *stable;
  symboltable *gtable;
  astnode *root;
};

struct astnode{
  token *token;
  int childcnt;
  astnode **child;
};

ast *createAst(tokenlist *tlist);
astnode *createAstNode(token *t);
void initAst(ast *tree,int start);
void expandAst(astnode *node,productionbody *prodbody);

ast *createast(tokenlist *tlist)
{
  ast *ret=(ast*)malloc(sizeof(ast));
  ret->stable=tlist->stable;
  ret->gtable=tlist->gtable;
  ret->root=NULL;
  return NULL;
}

astnode *createastnode(token *t)
{
  astnode *ret=(astnode*)malloc(sizeof(astnode));
  ret->token=t;
  ret->childcnt=0;
  ret->child=NULL;
  return ret;
}

void initast(ast *tree,int start)
{
  //astnode *root=createastnode(start);
  //tree->root=root;
}

void expandast(astnode *node,productionbody *prodbody)
{
  
}

#endif
