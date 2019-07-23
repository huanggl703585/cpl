#ifndef __TOKENLIST_H
#define __TOKENLIST_H

//for grammar analysis
#define IDENTIFIER  1
#define EQUIVALANCE 2

typedef struct tokenlist tokenlist;
struct tokenlist{
  union{
    int type;
    symboltable *table;
  }head;
  struct list_head list;
  int index;
};

#define inittokenlist(name,_table)				\
  tokenlist *name=(tokenlist*)malloc(sizeof(tokenlist));	\
  name->head.table=table;					\
  list_init(name->list)

#define _appendtokenlist(new,listhead)		\
  listaddtail(&(new->list),&(listhead->list))

tokenlist *createtokenlist(int type,int index);

#define appendtokenlist(listhead,type,index)		\
  do{							\
    tokenlist *__new=createtokenlist(type,index);	\
    _appendtokenlist(__new,listhead);			\
  }while(0)

tokenlist *createtokenlist(int type,int index)
{
  tokenlist *ret=(tokenlist*)malloc(sizeof(tokenlist));
  ret->head.type=type;
  list_init(ret->list);
  ret->index=index;
  return ret;
}


//=============================test
void printtokenlist(tokenlist *tlist)
{
  symboltable *table=tlist->head.table;
  tokenlist *pos;
  list_for_each_entry(pos,&(tlist->list),list){
    printf("%s ",table);
  }
}

#endif
