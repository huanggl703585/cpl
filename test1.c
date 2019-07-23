#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "utli.h"
#include "mstring.h"
int main()
{
  char *s="  hello world ";
  string *str=(string*)malloc(20);
  string ss=*str;
  initstring(ss,s,strlen(s));
}
