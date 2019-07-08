#ifndef __TESTRE_H
#define __TESTRE_H

#include "re.h"


void travelresymbol(re_symbol *sym)
{
  re_symbol *iter=sym;
  do{
    printf("%d \n",iter->index);
  }while((iter=list_next_entry(iter,list))!=sym);
}

//post order traversal
void travelretree(re_node *tree)
{
  if(isnode(tree)){
    if(tree->left!=NULL)
      travelretree(tree->left);
    if(tree->right!=NULL)
      travelretree(tree->right);
    printf("%d \n",tree->key.node->operator);
  }
  else
    printf("%d \n",tree->key.leave->index);
}

#endif
