#include "graph.h"

int main()
{
  graph *g=initgraph(5);

  graphaddedge(g,2,3);
  graphaddedge(g,1,4);
  graphaddedge(g,2,1);
  graphaddedge(g,1,3);
  graphaddedge(g,0,2);
  graphaddedge(g,0,4);

  //printnode(g->nodearr[0]);
  //printgraph(g);
  
  int result[5];
  topologysort(g,result);
  for(int i=0;i<5;i++)
    printf("%d ",result[i]);
}
