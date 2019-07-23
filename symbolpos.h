#ifndef __SYMBOLPOS_H
#define __SYMBOLPOS_H

#include "symboltable.h"

typedef struct symbolpos symbolpos;
struct symbolpos{
  symboltable *table;
  int id;
};

symbolpos* createsymbolpos(symboltable *table,int id);

symbolpos* createsymbolpos(symboltable *table,int id)
{
  symbolpos *ret=(symbolpos*)malloc(sizeof(symbolpos));
  ret->table=table;
  ret->id=id;
  return ret;
}

#define getsymbol(__symbolpos)				\
  searchsymbolbyid(__symbolpos->table,__symbolpos->id)

#endif
