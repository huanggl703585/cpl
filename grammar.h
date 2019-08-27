#ifndef __GRAMMAR_H
#define __GRAMMAR_H

//see book 2.2.1, it is a context-free grammar

#include "kvpair.h"
#include "symboltable.h"
#include "keyword.h"

typedef struct grammar grammar;
struct grammar{
  symboltable *table;
  int start;
  kwdict *kwdict;
};


#endif
