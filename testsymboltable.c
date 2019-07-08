#include <stdio.h>
#include "symboltable.h"
#include "random.h"

int main()
{
  char buf[20];
  symboltable *st=createsymboltable(symbol_table_basic_size);
  for(int i=0;i<30;i++){
    bzero(buf,20);
    sprintf(buf,"%d",i);
    insertsymboltable(st,buf);
  }

  for(int i=0;i<4096;i++){
    int index=st->hashtable[i];
    if(index==0) continue;
    printf("%s %d\n",st->symbolseq[index],index);
  }
  for(int i=0;i<st->count;i++){
    printf("%s %d\n",st->symbolseq[i],i);
  }

  
  //int index=st->hashtable[304];
  int index=1;
  printf("\n\n%s %d\n",st->symbolseq[index],index);
   

  for(int i=0;i<32;i++){
    //printf("  %s\n",(st->symbolseq)[1]);
    bzero(buf,20);
    //printf("  %s\n",(st->symbolseq)[1]);
    sprintf(buf,"%d",i);
    //printf("  %s\n",(st->symbolseq)[1]);
    printf("%d ",searchsymboltable(st,buf));
  }
  
}
