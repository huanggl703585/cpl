#include <stdio.h>
#include "avltree.h"

int main()
{
  //block_state arr[200];
  struct avltree *root=NULL;
  for(int i=0;i<20;i++){
    avltreeinsert(&root,i);
  }
  
  printf("\n\n");
  avltreeprint(root);
  //block_state *ret=container_of(&(arr[0].tree),block_state,tree);
  //printf("%p ",ret);
}
