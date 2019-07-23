#include "production.h"
#include <stdio.h>

int main()
{
  void *symbolattr=NULL;
  createpbody(symbolattr);
  appendpbody(symbolattr,3);
  appendpbody(symbolattr,7);
  appendpbody(symbolattr,9);
  printslist(symbolattr);
}
