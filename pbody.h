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

#define appendpbody(listhead,elem)		\
  do{						\
     initslist(__newpbody,elem);		\
     appendslist(__newpbody,listhead);		\
  }while(0)

#define getpbodykey(body)			\
  (int)(body->key)				

#define getpbodynext(body)			\
  list_next_entry(body,list)

#define getpbodyprev(body)			\
  list_last_entry(body,list)

#define pbodydrop(body)				\
  slist_drop(body)

void pbodyelimateparenthese(pbody *pos);

void pbodyelimateparenthese(pbody *pos)
{
  pbody *iter=pos;
  int key;
  while((key=getpbodykey(iter))!=')'){
    iter=getpbodynext(iter);
  }
  pbodydrop(pos);
  pbodydrop(iter);
}

#endif
