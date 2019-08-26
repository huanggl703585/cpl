#include "keyword.h"

int main()
{
  char *s1="hello";
  char *s2="world";
  char *s3="huang";

  kwdict *dict=createkwdicthead();
  kwdictinsert(dict,s1);
  kwdictinsert(dict,s2);
  printkwdict(dict);
  printf("%d %d",kwdictfind(dict,s2),kwdictfind(dict,s3));
}
