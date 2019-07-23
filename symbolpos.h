#ifndef __SYMBOLPOS_H
#define __SYMBOLPOS_H

#include "symboltable.h"

typedef struct symbolpos symbolpos;
struct symbolpos{
  symboltable *table;
  int id;
};

#endif
