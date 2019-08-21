#ifndef __TOKENIZER_H
#define __TOKENIZER_H

#include "dfa.h"
#include "symboltable.h"
#include "txtreader.h"
#include "mapinput.h"
#include "tokenlist.h"
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

//input must not have empty char

typedef struct tokenizer tokenizer;
struct tokenizer{
  reader *reader;
  symboltable* gtable; //it stores grammar
  symboltable* ttable; //it stores token
  dfa_instance *dfa;
  tokenlist *tlist;
};

tokenizer *createtokenizer(char *path,int gsize,int tsize)
{
  tokenizer *ret=(tokenizer*)malloc(sizeof(tokenizer));
  ret->reader=createreader(path,BLOCKSIZE);
  ret->gtable=createsymboltable(gsize,ASCII_BIAS);
  ret->ttable=createsymboltable(tsize,0);
  ret->tlist=createtokenlist(ret->gtable,ret->ttable);

  return ret;
}

#define charhavemapper(tokenizer,id)		\
  havemapper(tokenizer->gtable,id)

token* gettoken(tokenizer *worker)
{
  char c=' '; //in order to use skipspace(reader,c)
  reader* reader=worker->reader;
  token *ret;
  skipspace(reader,c);
  do{
    if(charhavemapper(worker,c)==0){
      if(walkdfa(worker->dfa,c)==0){
	int result=worker->dfa->lastend;
	char str[64];
	acceptword(worker->reader,str,64);
	if(result==0)
	  return NULL; //fail
	else{
	  ret=appendtoken(worker->tlist,str,result);
	  restartdfainstance(worker->dfa);
	  return ret;
	}
      }
    }
    else{
      charmapper *pos,*head=worker->gtable->terminal[c];
      int success=0;
      for_each_charmapper(pos,head){
	int input=(int)(pos->key);
	dfa_instance copyright;
	dfainstancecopy(worker->dfa,(&copyright));
	if(walkdfa(worker->dfa,input)==0){
	  if(worker->dfa->lastend==0)
	    dfainstanceundo((&copyright),worker->dfa);
	  else{
	    success=1;
	    int result=worker->dfa->lastend;
	    char str[64];
	    acceptword(worker->reader,str,64);
	    ret=appendtoken(worker->tlist,str,result);
	    restartdfainstance(worker->dfa);
	    return ret;
	  }
	}
	else{
	  success=1;
	  break;
	}
      }
      if(success==0)
	return NULL; //fail
    }
  }while((c=readone(reader))!='\0');
  return NULL;
}

void _dotokenizer(tokenizer *worker)
{
  char c=0;
  reader* reader=worker->reader;
  assert(reader!=NULL);
  //c=readone(reader);
  while((c=readone(reader))!='\0'){     
    if(charhavemapper(worker,c)==0){
      if(walkdfa(worker->dfa,c)==0){
	int result=worker->dfa->lastend;
	char str[64];
	acceptword(worker->reader,str,64);
	if(result==0){ 
	  if(isspace(c))
	    acceptword(reader,&c,1);
	  else
	    assert(0); //fail
	}
	else
	  appendtoken(worker->tlist,str,result);
	restartdfainstance(worker->dfa);
      }
    }
    else{
      charmapper *pos,*head=worker->gtable->terminal[c];
      int success=0;
      for_each_charmapper(pos,head){
	int input=(int)(pos->key);
	dfa_instance copyright;
	dfainstancecopy(worker->dfa,(&copyright));
	if(walkdfa(worker->dfa,input)==0){
	  if(worker->dfa->lastend==0)
	    dfainstanceundo((&copyright),worker->dfa);
	  else{
	    success=1;
	    int result=worker->dfa->lastend;
	    char str[64];
	    acceptword(worker->reader,str,64);
	    appendtoken(worker->tlist,str,result);
	    restartdfainstance(worker->dfa);
	  }
	}
	else{
	  success=1;
	  break;
	}
      }
      if(success==0){ 
	if(isspace(c))
	  acceptword(reader,&c,1);
	else
	  assert(0);  //fail
	restartdfainstance(worker->dfa);
      }
    }
    
    if(isspace(c)==1){
      acceptword(reader,&c,1);
    }
  }
}

#endif
