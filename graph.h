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

//#define graphaddedge(g,from,to)	 
void nodeaddedge(node *from,node *to);  
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

void nodeaddedge(node *from,node *to)
{  
  node *new=createnode(from->index);
  listaddtail(&(new->edge),&(to->edge));
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

//==========================stack version

//in edge-list, the node index is the index of the nodearr
#define initnode(node,_index)			\
  node.index=_index;				\
  list_init(node.edge)				\

#define arraynodeaddedge(_nodearr,_from,_to,_pt)		\
  do{								\
    initnode(_nodearr[_pt],_from);				\
    listaddtail(&(_nodearr[_pt].edge),&(_nodearr[_to].edge));	\
    _pt++;							\
  }while(0)

void arraytopologysort(node nodearr[],int size,int result[]);
void arraytopologysortsignfirst(node nodearr[],int size,int result[],int *sign);

void arraytopologysort(node nodearr[],int size,int result[])
{
  int mark[size];
  int tmpmark[size];
  bzero(mark,size*sizeof(int));
  int resultpt=0;
  while(resultpt<size){
    bzero(tmpmark,size*sizeof(int));
    for(int i=0;i<size;i++){
      if(mark[i]==0){
	node *pos;
	int canpush=1;
	list_for_each_entry(pos,&(nodearr[i].edge),edge){
	  if(mark[pos->index]==0)
	    canpush=0;
	}
	if(canpush){
	  tmpmark[i]=1;
	  result[resultpt++]=i;
	}
      }
    }
    for(int i=0;i<size;i++){
      if(tmpmark[i]==1)
	mark[i]=1;
    }
  }
}

void arraytopologysortsignfirst(node nodearr[],int size,int result[],int *sign)
{
  int mark[size];
  int tmpmark[size];
  int first=0;
  bzero(mark,size*sizeof(int));
  int resultpt=0;
  while(resultpt<size){
    bzero(tmpmark,size*sizeof(int));
    for(int i=0;i<size;i++){
      if(mark[i]==0){
	node *pos;
	int canpush=1;
	list_for_each_entry(pos,&(nodearr[i].edge),edge){
	  if(mark[pos->index]==0)
	    canpush=0;
	}
	if(canpush){
	  tmpmark[i]=1;
	  result[resultpt++]=i;
	}
      }
    }
    for(int i=0;i<size;i++){
      if(tmpmark[i]==1)
	mark[i]=1;
    }
    if(first==0){
      first=1;
      *sign=resultpt;
    }
  }
}
//==========================test/print
void printnode(node *n);

void printnode(node *n)
{
  node *pt=n;
  printf("nodeindex %d\n",n->index);
  list_for_each_entry(pt,&(n->edge),edge){
    printf("%d %p\n",pt->index,pt);
  } 
  printf("\n");
}

void printnodearr(node nodearr[],int size)
{
  for(int i=0;i<size;i++)
    printnode(&(nodearr[i]));
}

void printgraph(graph *g)
{
  int size=g->nodenum;
  for(int i=0;i<size;i++)
    printnode((g->nodearr[i]));
}



#endif
