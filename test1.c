#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltable.h"

int main()
{
  char *hw="world";
  symbolderivename(hello,hw);
  printf("%d %s",strlen(hello),hello);
}
