#ifndef __KVPAIR_H
#define __KVPAIR_H

#include "list.h"

typedef struct kvpair kvpair;
struct kvpair{
  void *key;
  void *value;
  struct list_head list;
};



#endif
