#ifndef __SET_H
#define __SET_H

#include <stdio.h>
#include "avltree.h"
typedef struct avltree set;

#define insertset(set,key) avltreeinsert(&set,key) 
#define findset(set,key) avltreefind(set,key)
#define printset(set) avltreeprint(set)

void intersectset(set **s1,set *s2)
{
  if(s2==NULL){
    return ;
  }
  set* arr[256];
  arr[0]=s2;
  int pt=0;

  while(pt>=0){
    set *cur=arr[pt];
    pt--;
    printf("%d ",cur->key);
    //_avltreeinsert(s1,arr[pt]);
    insertset(*s1,cur->key);
    if(cur->left!=NULL)
      arr[++pt]=cur->left;
    if(cur->right!=NULL)
      arr[++pt]=cur->right;
  }
  //printf("\n");
}

#endif
