#include "option.h"
#include <stdio.h>

int main(int argc,char **argv)
{
  init_option(option);
  //lexical grammar, text and structural grammar respectively
  char *input[3];
  int pt=0;
  
  while(*++argv!=NULL){
    if((*argv)[0]=='-'){

    }
    else
      input[pt++]=(*argv);
  }

  int inputerror;
  if((inputerror=checkinput(option,input))){
    
  }
  
  execute(option,input);
}
