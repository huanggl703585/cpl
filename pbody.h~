#ifndef __PBODY_H
#define __PBODY_H

//we assume the the head of pbody is 0(NULL)  
typedef slist pbody;

#define pbody_for_each(pos,listhead)		\
  slist_for_each(pos,listhead)

#define createpbody(pos) 					\
  do{									\
    initslist(__newpbody,NULL);						\
    pos=__newpbody;							\
  }while(0)

void appendpbody(pbody *listhead,int elem)
{
  initslist(tmp,elem);
  appendslist(tmp,listhead);
}
/*
#define appendpbody(listhead,elem)		\
  do{						\
     initslist(__newpbody,elem);		\
     appendslist(__newpbody,listhead);		\
  }while(0)
*/
#define getpbodykey(body)			\
  (int)(body->key)				

#define getpbodynext(body)			\
  list_next_entry(body,list)

#define getpbodyprev(body)			\
  list_last_entry(body,list)

#define pbodydel(body)				\
  slist_del(body)

void pbodyappendlistwithlen(pbody *listhead,pbody *newlist,int len)
{
  int tmp=len;						
  pbody *pos=newlist;						
  while(tmp--){						
    pos=getpbodynext(pos);					
    int key=getpbodykey(pos);					
    appendpbody(listhead,key);				
  }
}

#endif
