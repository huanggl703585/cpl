#ifndef __TESTGRAPH_H
#define __TESTGRAPH_H

#include <stdio.h>
#include "graph.h"

void printedge(node *n);
void printgraph(graph *g);

void printnode(node *n)
{
  node *pt=n->edge;
  while((pt=list_next_entry(pt,edge))!=n)
    printf("%d %p\n",pt->index,pt);
  } 
  printf("\n");
}

void printgraph(graph *g)
{
  int size=g->nodenum;
  for(int i=0;i<size;i++)
    printnode((g->nodearr[i]));
}

#endif
