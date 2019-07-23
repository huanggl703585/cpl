#ifndef __SYMBOLATTR_H
#define __SYMBOLATTR_H

#include "graph.h"
#include "production.h"
#include "mapinput.h"

#define NONTERMINAL1 1
#define NONTERMINAL2 2
#define TERMINAL     3

struct symbolattr;
typedef struct symbolattr symbolattr;

struct symbolattr{
  int type;
  union{
    production *production;
    charmapper *mapper;
  }attr;
  node *node;
};

#endif
