#include "txtreader.h"
#include "mapinput.h"
#include <ctype.h>
#include <stdio.h>

char *path="cgrammer.txt";

int main()
{
  reader* r=createreader(path,BLOCKSIZE);
  char c;
  char readbuf[11];
  int cnt=0;
 
  charmapper head;
  charmapper m1,m2;
  initcharmapper(head,0,NULL,0);
  initcharmapper(m1,2,isalpha,1);
  initcharmapper(m2,1,isascii,2);
  addcharmapper(&m1,&head);
  addcharmapper(&m2,&head);

  do{
    c=readone(r);
    /*cnt++;
    if(cnt==10){
      bzero(readbuf,11);
      acceptword(r,readbuf,10);
      readbuf[10]='\0';
      printf("%s\n",readbuf);
      cnt=0;
      }*/
    printf("%d",mapinput(&head,c));
    //mapinput(&head,c);
  }while(c!='\0');
}
