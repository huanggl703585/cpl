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
    printf("%c",c);
    //mapinput(&head,c);
  }while(c!='\0');
}
