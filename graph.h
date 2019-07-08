#ifndef __GRAPH_H
#define __GRAPH_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "set.h"

typedef struct node node;
typedef struct graph graph;

struct node{
  int index;
  struct list_head edge; //store 'from' node, node itself is 'to' node
};

struct graph{
  int nodenum;
  node **nodearr;  
};

graph* initgraph(int size);
node* createnode(int index);

void graphaddedge(graph *g,int from,int to);

void topologysort(graph *g,int result[]); //result's size == g->nodenum

graph* initgraph(int size)
{
  graph* ret=(graph*)malloc(sizeof(graph));
  ret->nodenum=size;
  ret->nodearr=(node**)malloc(sizeof(node*)*size);
  for(int i=0;i<size;i++)
    ret->nodearr[i]=createnode(i);

  return ret;
}

node* createnode(int index)
{
  node* ret=(node*)malloc(sizeof(node));
  ret->index=index;
  list_init(ret->edge);
  return ret;
}

void graphaddedge(graph *g,int from,int to)
{
  node *f=g->nodearr[from];
  node *t=g->nodearr[to];
  node *new=createnode(f->index);
  listaddtail(&(new->edge),&(t->edge));
}

void topologysort(graph *g,int result[])
{
  set *readed=NULL;
  int cnt=0;
  while(cnt<g->nodenum){
    for(int i=0;i<g->nodenum;i++){
      node *temp=g->nodearr[i];
      if(findset(readed,temp->index))
	continue;
      node *iter=temp;
      while((iter=list_next_entry(iter,edge))!=temp){
	if(!findset(readed,iter->index))
	  goto nextnode; 
      }
      //success, this node's 'in' is zero
      insertset(readed,temp->index);
      result[cnt]=temp->index;
      cnt++;
    nextnode:
      ;
    }
  }
}

void printnode(node *n)
{
  node *pt=n;
  printf("nodeindex %d\n",n->index);
  while((pt=list_next_entry(pt,edge))!=n){
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
