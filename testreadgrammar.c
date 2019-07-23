#include "txtreader.h"
#include "symboltable.h"
#include "tokenlist.h"

int main()
{
  inittokenlist(tlist);
  reader *r=createreader("cgrammer.txt",4096);
  char c;
  char buf[64];
  int readflag=0;
  do{
    c=readone(r);
    //WE USE IT TO INSTEAD DFA
    if(isspace(c)){
      if(readflag==0)
	acceptword(r,buf,64);
      else if(readflag==1){
	bzero(buf,64);
	int len=acceptword(r,buf,64);	
	printf("%s\n",buf);
	readflag=0;
	appendtokenlist(tlist,IDENTIFIER,buf,len);
      }
      else if(readflag==2){
	bzero(buf,64);
	int len=acceptword(r,buf,64);
	readflag=0;
	appendtokenlist(tlist,EQUIVALANCE,buf,len);
      }
    } 
    else if(isalnum(c)){
      readflag=1;
    }
    else if(ispunct(c))
      readflag=2;
  }while(c!='\0');
  printtokenlist(tlist);
}
