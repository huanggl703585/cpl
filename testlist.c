#include "list.h"
#include <stdio.h>
#include "kvpair.h"

int charcmp(void* a,void* b)
{
  int ta=(int)a,tb=(int)b;
  char ca=(char)ta,cb=(char)tb;
  if(ca==cb) return 0;
  if(ca>cb) return 1;
  else return -1;
}

int charisequal(void* a,void* b)
{
  int ta=(int)a,tb=(int)b;
  char ca=(char)ta,cb=(char)tb;
  return ca==cb;
}

int main()
{
  int a=1,b=5,c=4,d=3,m=100;
  kvpair c0,c1,c2,c3,c4;
  kvpair *k,*k1,*k2,*k3,*k4,*tmp;
  k=&c0;
  k1=&c1;k2=&c2;k3=&c3;k4=&c4;
  //k->key=(void*)m;
  k1->key=(void*)a;
  k2->key=(void*)b;
  k3->key=(void*)c;
  k4->key=(void*)d;
  list_init(k->list);
  list_init(k1->list);
  list_init(k2->list);
  list_init(k3->list);
  list_init(k4->list);

  list_insert_order(tmp,k,list,key,k1,charcmp);
  printf("\n");
  list_insert_order(tmp,k,list,key,k2,charcmp);
  printf("\n");
  list_insert_order(tmp,k,list,key,k3,charcmp);
  printf("\n");
  list_insert_order(tmp,k,list,key,k4,charcmp);
  //list_find_key(tmp,&(k1->list),list,key,k2,charcmp);
  //list_for_each_entry_withouthead(tmp,&(k1->list),list){
  //printf("%d ",(int)(tmp->key));
  //}
  //listadd(&(k2->list),&(k1->list));
  printf("\n");
  list_for_each_entry(tmp,&(k->list),list){
    printf("%d ",(int)tmp->key);
  }

  list_find_key(tmp,k,list,key,3,charisequal);
  printf("\n%d ",(int)(tmp->key)); 
  list_find_key(tmp,k,list,key,6,charisequal);
  printf("\n%d ",(int)(tmp->key));
}
