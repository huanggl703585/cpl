#include "utli.h"
#include "avltree.h"
#include <stdio.h>

int main()
{
  int len1=5,len2=6,len3=6;
  int arr[5];
  bzero(arr,sizeof(int)*5);
  arraysortinsert(arr,5,2);
  printf("\n");
  arraysortinsert(arr,5,4);
  arraysortinsert(arr,5,1);
  arraysortinsert(arr,5,8);
  printf("\n");
  arraysortinsert(arr,5,9);
  arraysortinsert(arr,5,6);
  
  int arr2[5];
  bzero(arr2,sizeof(int)*5);
  arraysortinsert(arr2,5,4);
  printf("\n");
  arraysortinsert(arr2,5,8);
  arraysortinsert(arr2,5,9);
  arraysortinsert(arr2,5,2);
  printf("\n");
  arraysortinsert(arr2,5,1);
  arraysortinsert(arr2,5,6);

  printf("%d ",intarrayisequal(arr,arr2,5,5));

  int a[6]={2,7,5,9,1,3};
  int b[6]={1,5,9,3,7,2};

  int res=arrayhaszero(a,6,intiszero);
  printf("res %d \n",res);

  struct avltree *t1=NULL,*t2=NULL;
  for(int i=0;i<6;i++){
    avltreeinsert(&t1,a[i]);
    avltreeinsert(&t2,b[i]);
  }
  printf("\n");
  avltreeprint(t1);
  avltreeprint(t2);
  printf("%d ",avltreecmp(t1,t2));
}
