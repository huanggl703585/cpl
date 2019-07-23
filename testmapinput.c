#include "mapinput.h"
#include <stdio.h>
#include <ctype.h>

int main()
{
  charmapper head;
  charmapper m1,m2,m3;
  initcharmapper(head,0,NULL,0);
  initcharmapper(m1,1,isalpha,1);
  initcharmapper(m2,1,isdigit,2);
  initcharmapper(m3,1,isspace,3);

  addcharmapper(&m1,&head);
  addcharmapper(&m2,&head);
  addcharmapper(&m3,&head);

  char c;
  c=' ';

  printf("%d ",mapinput(&head,c));
}
