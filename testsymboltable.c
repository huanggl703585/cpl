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
    insertsymboltable(st,buf,NULL);
  }

  for(int i=0;i<1024;i++){
    symbolitem *item=st->table[i];
    if(item==NULL) continue;
    printf("%s %d\n",item->name,item->id);
  }

  /*
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
  */
}
