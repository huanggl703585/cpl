#include "production.h"
#include <stdio.h>

int main()
{
  /*void *symbolattr=NULL;
  createpbody(symbolattr);
  appendpbody(symbolattr,3);
  appendpbody(symbolattr,7);
  appendpbody(symbolattr,9);
  printslist(symbolattr);*/

  production *p=createproduction(2);
  appendprodrange(p,2,6);
  printproduction(p);
}
