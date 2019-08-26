#ifndef __LIST_H
#define __LIST_H

#include "utli.h"
#include <stdio.h>

struct list_head{
  struct list_head *next;
  struct list_head *prev;
};

#define LIST_HEAD_INIT(name) {&(name),&(name)}
#define list_init(name)					\
  name.next=&name;name.prev=&name
#define SLIST_HEAD_INIT(name) {&(name)}

#define listisempty(list)			\
  (list.prev==(&list) && list.next==(&list))

void __list_add(struct list_head *new,
		struct list_head *next,
		struct list_head *prev)
{
  new->prev=prev;
  prev->next=new;
  new->next=next;
  next->prev=new;
}

void listadd(struct list_head *new,
	     struct list_head *prev)
{
  __list_add(new,prev->next,prev);
}

void listaddtail(struct list_head *new,
		 struct list_head *next)
{
  __list_add(new,next,(next->prev));
}

void __list_drop(struct list_head *prev,
		struct list_head *next)
{
  prev->next=next;
  next->prev=prev;
}

void listdrop(struct list_head *l)
{
  __list_drop(l->prev,l->next);
}
   
void listreplace(struct list_head *newhead,struct list_head *origin)
{
  origin->prev->next=newhead->next;
  origin->next->prev=newhead->prev;
  newhead->next->prev=origin->prev;
  newhead->prev->next=origin->next;
  //origin->prev=origin->next=origin;
  //newhead->prev=newhead->next=newhead;
}

void listappend(struct list_head *head,struct list_head *new)
{
  head->prev->next=new->next;
  new->next->prev=head->prev;
  head->prev=new->prev;
  new->prev->next=head;
}

#define list_entry(ptr,type,member)		\
  container_of(ptr,type,member)

#define list_for_each(pos,head)			\
  for(pos=(head)->next;pos!=(head);pos=pos->next)


#define list_entry(ptr,type,member)		\
  container_of(ptr,type,member)

#define list_first_entry(ptr,type,member)	\
  list_entry((ptr)->next,type,member)

#define list_last_entry(ptr,type,member)	\
  list_entry((ptr)->prev,type,member)

#define list_next_entry(pos,member)			\
  list_entry((pos)->member.next,typeof(*pos),member)  

#define list_prev_entry(pos,member)			\
  list_entry((pos)->member.prev,typeof(*pos),member)

//head is a ptr
#define list_for_each_entry(pos,head,member)		\
  for(pos=list_first_entry(head,typeof(*pos),member);	\
      &pos->member!=(head);				\
      pos=list_next_entry(pos,member))

//the list is ordered
#define list_find_key(pos,listhead,member,keymember,target,isequal)	\
  do{									\
    list_for_each_entry(pos,&(listhead->member),member){		\
      if(isequal((void*)target,pos->keymember))				\
	break;								\
    }									\
  }while(0)

//from small to large
#define list_cmp_key(pos,head,member,keymember,target,cmp)		\
  list_for_each_entry(pos,head,member){					\
    if(cmp(target->keymember,pos->keymember)<0)				\
      break;								\
  }									\
  pos=list_last_entry(&(pos->member),typeof(*pos),member)

//ignore it
//if &(pos->member)==head, can't find relative key

//listhead is different from list above
#define list_insert_order(listhead,member,keymember,new,cmp)		\
  do{									\
    typeof(listhead) __pos;						\
    list_cmp_key(__pos,&(listhead->member),member,keymember,new,cmp);	\
    listadd(&(new->member),&(__pos->member));				\
  }while(0)

#endif

  
