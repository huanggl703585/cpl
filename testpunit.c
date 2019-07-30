#include "symboltable.h"
#include "production.h"

int main()
{
  production* ppos;
  productionbody *pbpos;

  

  ppos=createproduction(2);

  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,'(');
  appendprodbody(pbpos,4);
  appendprodbody(pbpos,')');

  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,3);
  appendprodbody(pbpos,'(');
  appendprodbody(pbpos,4);
  appendprodbody(pbpos,')');

  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,3);
  appendprodbody(pbpos,'(');
  appendprodbody(pbpos,4);
  appendprodbody(pbpos,')');

  pbody *p1head=pbpos->body;
  pbody *p1=pbpos->body;
  pbody *res1,*res2;
  int len1,len2;
  pbodygetnextunit(p1,p1head,res1,len1);
  pbodygetnextunit(p1,p1head,res2,len2);
  printpbodyunit(res1,len1);
  printpbodyunit(res2,len2);
  
  pbpos=createproductionbody(ppos);
  appendprodbody(pbpos,'(');
  appendprodbody(pbpos,4);
  appendprodbody(pbpos,')');

  pbody *p2head=pbpos->body;
  pbody *p2=p2head;
  pbody *res3,*res4;
  int len3,len4;
  pbodygetnextunit(p2,p2head,res3,len3);
  pbodygetnextunit(p2,p2head,res4,len4);
  printpbodyunit(res3,len3);
  printpbodyunit(res4,len4);

  int res;
  res=pbodyunitequal(res1,len1,res3,len3);
  printf("%d \n",res);
  res=pbodyunitequal(res2,len2,res3,len3);
  printf("%d \n",res);
 
  prodfindlcp(ppos,mark,resarray,lcp,len,restlen);
  printf("\n%d\n",len);
  printpbodyunit(lcp,len);
  for(int i=0;i<ppos->cnt;i++)
    printf("%d %d\n",mark[i],restlen[i]);

}
