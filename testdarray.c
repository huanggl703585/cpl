#include "dynamicarray.h"
#include "utli.h"



int main()
{
  int arr[10]={0};
  int varr[10]={0};
  kvpair kvarr[10];
  for(int i=0;i<10;i++){
    arr[i]=(7*(i+1))%5;
    varr[i]=2*arr[i];
    kvarr[i].key=(void*)(arr[i]);
    kvarr[i].value=(void*)(varr[i]);
  }
  
  darray *darr=createdarray(2,2);
  for(int i=0;i<10;i++)
    sortinsertdarray(darr,(void*)(&(kvarr[i])),intkvpaircmp);
  
  kvpair *object=(kvpair*)sortfinddarray(darr,(void*)3,intkvpairkeycmp);
  printf("%d %d\n",(int)(object->key),(int)(object->value));
}
