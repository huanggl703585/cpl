#ifndef __SET_H
#define __SET_H

#include <stdio.h>
#include "avltree.h"
typedef struct avltree set;

#define insertset(set,key) avltreeinsert(&set,key) 
//if find, return 1 else return 0
#define findset(set,key) avltreefind(set,key)
#define printset(set) avltreeprint(set)

int intersectset(set **s1,set *s2)
{
  if(s2==NULL){
    return 0;
  }
  set* arr[256];
  arr[0]=s2;
  int pt=0;
  int ret=0;

  while(pt>=0){
    set *cur=arr[pt];
    pt--;
    //printf("%d ",cur->key);
    //_avltreeinsert(s1,arr[pt]);
    ret+=(insertset(*s1,cur->key));
    if(cur->left!=NULL)
      arr[++pt]=cur->left;
    if(cur->right!=NULL)
      arr[++pt]=cur->right;
  }
  return ret;
}

#endif
