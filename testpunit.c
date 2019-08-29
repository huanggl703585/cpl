
#include "production.h"

int main()
{
  production* ppos;
  productionbody *pbpos;

  ppos=createProduction(2);

  pbpos=createProdbody(ppos);
  appendProdbody(pbpos,'(');
  appendProdbody(pbpos,4);
  appendProdbody(pbpos,')');

  pbpos=createProdbody(ppos);
  appendProdbody(pbpos,3);
  appendProdbody(pbpos,'(');
  appendProdbody(pbpos,4);
  appendProdbody(pbpos,')');

  pbpos=createprodbody(ppos);
  appendProdbody(pbpos,3);
  appendProdbody(pbpos,'(');
  appendProdbody(pbpos,4);
  appendProdbody(pbpos,')');

  pbody *p1head=pbpos->body;
  pbody *p1=pbpos->body;
  pbody *res1,*res2;
  int len1,len2;
  pbodyGetNextUnit(p1,p1head,res1,len1);
  pbodyGetNextUnit(p1,p1head,res2,len2);
  printPbodyunit(res1,len1);
  printPbodyunit(res2,len2);
  
  pbpos=createProdbody(ppos);
  appendProdbody(pbpos,'(');
  appendProdbody(pbpos,4);
  appendProdbody(pbpos,')');

  pbody *p2head=pbpos->body;
  pbody *p2=p2head;
  pbody *res3,*res4;
  int len3,len4;
  pbodyGetNextUnit(p2,p2head,res3,len3);
  pbodyGetNextUnit(p2,p2head,res4,len4);
  printPbodyunit(res3,len3);
  printPbodyunit(res4,len4);

  int res;
  res=pbodyunitequal(res1,len1,res3,len3);
  printf("%d \n",res);
  res=pbodyunitequal(res2,len2,res3,len3);
  printf("%d \n",res);
 
  prodfindlcp(ppos,mark,resarray,lcp,len,restlen);
  printf("\n%d\n",len);
  printpbodyunit(lcp,len);
  for(int i=0;i<ppos->cnt;i++)
    printf("%d %d",mark[i],restlen[i]);

}
