#ifndef __TOKENLIST_H
#define __TOKENLIST_H

//for grammar analysis
#define IDENTIFIER  1
#define EQUIVALANCE 2

typedef struct tokenlist tokenlist;
struct tokenlist{
  int type;
  struct list_head list;
  int len;
  char str[];
};

#define inittokenlist(name)					\
  tokenlist *name=(tokenlist*)malloc(sizeof(tokenlist));	\
  list_init(name->list)

#define _appendtokenlist(new,listhead)		\
  listaddtail(&(new->list),&(listhead->list))

tokenlist *createtokenlist(int type,char *str,int len);

#define appendtokenlist(listhead,type,str,len)		\
  do{							\
    tokenlist *__new=createtokenlist(type,str,len);	\
    _appendtokenlist(__new,listhead);			\
  }while(0)

tokenlist *createtokenlist(int type,char *str,int len)
{
  tokenlist *ret=(tokenlist*)malloc(sizeof(tokenlist)+len+1);
  ret->type=type;
  list_init(ret->list);
  ret->len=len;
  memcpy(ret->str,str,len+1);
  return ret;
}


//=============================test
void printtokenlist(tokenlist *tlist)
{
  tokenlist *pos;
  list_for_each_entry(pos,&(tlist->list),list){
    printf("%s ",pos->str);
  }
}

#endif
