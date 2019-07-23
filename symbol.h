#ifndef __SYMBOL_H
#define __SYMBOL_H

typedef struct terminalattr terminalattr;
typedef struct nonterminalattr nonterminalattr;

struct terminalattr{
  char *content;
  int len;
  int index;  //index in symboltable
};

struct nonterminalattr{
  darray *pset; //production set
  int index;
};


#endif
