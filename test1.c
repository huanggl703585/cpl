#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltable.h"

int main()
{
  char *hw="world";
  symbolderivename(hello,hw,3);
  printf("%d %s",strlen(hello),hello);
}
