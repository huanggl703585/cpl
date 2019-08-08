#ifndef __PBODYUNIT_H
#define __PBODYUNIT_H

#include "pbody.h"

#define SYMBOL 1  //a symbol, like identifier, token
#define SINGLE 2  //a terminal, like 'a', '|'
#define PARENT 3  //a group of symbol in a pair of parentheses, liks (ab)

typedef struct pbodyunit pbodyunit;
struct pbodyunit{
  pbody *body;
  int len;
  int type;
};

#define pbodygetnextunit(body,head,res,len)		\
  do{							\
    res=body;						\
    body=list_next_entry(body,list);			\
    int key=(int)(body->key);				\
    len=1;						\
    if(key=='('){					\
      while(key!=')'){					\
	body=list_next_entry(body,list);		\
	if((body)==(head)){				\
	  res=NULL;					\
	  break;					\
	}						\
	key=getpbodykey(body);				\
	len++;						\
      }							\
    }							\
    else if(key=='\''){					\
      							\
    }							\
  }while(0)

int pbodyunitequal(pbody *u1,int ulen1,pbody *u2,int ulen2);
void pbodyunitdel(pbody *body,int len);
void printpbodyunit(pbody *body,int len);
void pbodyunittestelimateparenthese(pbody *listhead,int *len);

int pbodyunitequal(pbody *u1,int ulen1,pbody *u2,int ulen2)
{
  if(ulen1!=ulen2) return 0;
  pbody *tmp1=u1,*tmp2=u2;
  int key1,key2;
  int tmp=ulen1;
  while(tmp--){
    tmp1=getpbodynext(tmp1);
    tmp2=getpbodynext(tmp2);
    key1=getpbodykey(tmp1);
    key2=getpbodykey(tmp2);
    if(key1!=key2) return 0;
  }
  return 1;
}

void pbodyunitdel(pbody *body,int len)
{
  pbody *pos=body;
  pbody *tmp=pos=getpbodynext(pos);
  while(len--){
    tmp=getpbodynext(tmp);
    pbodydel(pos);
    pos=tmp;
  }
}
    
void printpbodyunit(pbody *body,int len)
{
  printf("len %d\t",len);			
  pbody* pos=getpbodynext(body);		
  int tmp=len;				
  while(tmp--){				
    int key=getpbodykey(pos);			
    printf("%d ",key);			
    pos=getpbodynext(pos);			
  }						
  printf("\n");				
}

void pbodyunittestelimateparenthese(pbody *listhead,int *len)
{
  pbody *first=getpbodynext(listhead);
  pbody *last=first;
  int cnt=*len;
  while(--cnt)
    last=getpbodynext(last);

  int key1=getpbodykey(first),key2=getpbodykey(last);
  if(!(key1=='(' && key2==')'))
    return ;

  pbodydel(first);
  pbodydel(last);
  *len-=2;
}

#endif
