#include "graph.h"

int main()
{
  /*
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
  */

  node nodearr[256];
  int nodenum=10;
  int pt=nodenum;
  for(int i=0;i<nodenum;i++){
    initnode(nodearr[i],i+5);
  }
  arraynodeaddedge(nodearr,1,3,pt);
  arraynodeaddedge(nodearr,1,4,pt);
  arraynodeaddedge(nodearr,2,7,pt);
  arraynodeaddedge(nodearr,3,4,pt);
  arraynodeaddedge(nodearr,3,7,pt);
  arraynodeaddedge(nodearr,7,8,pt);
  
  int result[nodenum];
  int first;
  arraytopologysortsignfirst(nodearr,nodenum,result,&first);
  printf("%d\n",first);
  for(int i=0;i<nodenum;i++)
    printf("%d ",result[i]);
}
