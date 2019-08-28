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

typedef struct tokenizer tokenizer;
struct tokenizer{
  reader *reader;
  symboltable* gtable; //it stores grammar
  symboltable* ttable; //it stores token
  dfa_instance *dfa;
  tokenlist *tlist;
};

#define tokenizerFail(c) {			\
  printf("failed at %c\n",c);			\
  assert(0); }					

#define charHaveMapper(tokenizer,id)		\
  haveMapper(tokenizer->gtable,id)

tokenizer *createTokenizer(char *path,int gsize,int tsize);
token* getToken(tokenizer *worker);
void _doTokenizer(tokenizer *worker);
void printTokenlistWithName(tokenizer *tokenizer);

tokenizer *createTokenizer(char *path,int gsize,int tsize)
{
  tokenizer *ret=(tokenizer*)malloc(sizeof(tokenizer));
  ret->reader=createreader(path,BLOCKSIZE);
  ret->gtable=createSymboltable(gsize,ASCII_BIAS);
  ret->ttable=createSymboltable(tsize,0);
  ret->tlist=createTokenlist(ret->gtable,ret->ttable);

  return ret;
}

token* getToken(tokenizer *worker)
{
  char c; //in order to use skipspace(reader,c)
  reader* reader=worker->reader;
  token *ret;
  
  while((c=readone(reader))!='\0'){
    if(dfainstartstate(worker->dfa))
      skipspace(worker->reader,c);

    if(charHaveMapper(worker,c)==0){
      if(walkdfa(worker->dfa,c)==0){
	int result=worker->dfa->lastend;
	if(result==0){
	  tokenizerFail(c);
	}
	char str[64];
	acceptword(worker->reader,str,64);
	ret=appendToken(worker->tlist,str,result);
	restartdfainstance(worker->dfa);
	//printf("%s\n",str);
	return ret;
      }
    }
    else{  //if have charmapper
      charmapper *pos,*head=worker->gtable->terminal[c];
      int canextend=0;
      int inputrecord;
      dfa_instance copyright;
      dfainstancecopy(worker->dfa,(&copyright));
      for_each_charmapper(pos,head){
	int input=(int)(pos->key);
	if((walkdfa(worker->dfa,input))!=0){
	  canextend=1;
	  break;
	}
	else{ //walkdfa(worker->dfa,input)==0)
	  if(worker->dfa->lastend!=0){
	    canextend=-(worker->dfa->lastend);
	    inputrecord=input;
	  }
	  dfainstanceundo((&copyright),worker->dfa);
	}
      } //<-end of for_each
      if(canextend==1)
	continue;
      if(canextend<0){
	walkdfa(worker->dfa,inputrecord);
	char str[64];
	acceptword(worker->reader,str,64);
	ret=appendToken(worker->tlist,str,-canextend);
	restartdfainstance(worker->dfa);
	//printf("%s\n",str);
	return ret;
      }
      tokenizerFail(c);
    }
  }
  return NULL;
}

void _doTokenizer(tokenizer *worker)
{
  char c=' ';
  reader* reader=worker->reader;
  assert(reader!=NULL);

  while((c=readone(reader))!='\0'){
    //if dfa is in start state, we can skip the space safely
    if(dfainstartstate(worker->dfa))
      skipspace(worker->reader,c);
    
    if(charHaveMapper(worker,c)==0){
      if(walkdfa(worker->dfa,c)==0){
	int result=worker->dfa->lastend;
	if(result==0){ 
	  tokenizerFail(c); //fail
	}
	char str[64];
	acceptword(worker->reader,str,64);
	//printf("%s\n",str);
	appendToken(worker->tlist,str,result);
	restartdfainstance(worker->dfa);
      }
    }
    else{
      charmapper *pos,*head=worker->gtable->terminal[c];
      int inputrecord;
      int canextend=0;
      dfa_instance copyright;
      dfainstancecopy(worker->dfa,&(copyright));
      //for each input:				
      //case 1: walkdfa()!=0 : can extend, take the input directly
      //case 2: walkdfa()==0 && lastend!=0, it means there is a token, take that input if any case 1 to every input
      //case 3: walkdfa()==0 && lastend==0, it means dfa is stopped and no token, then throw a fault if eveny input is in case 3
      for_each_charmapper(pos,head){
	int input=(int)(pos->key);
	if((walkdfa(worker->dfa,input))!=0){
	  canextend=1;
	  break;
	}
	else{  //((walkdfa(worker->dfa,input))==0)
	  if(worker->dfa->lastend!=0){ //correct input
	    canextend=-(worker->dfa->lastend);
	    inputrecord=input;
	  }
	  dfainstanceundo((&copyright),worker->dfa);
	}
      } //<-end of for_each
      if(canextend==1)
	continue;
      if(canextend<0){ //new word
	walkdfa(worker->dfa,inputrecord);
	char str[64];
	acceptword(worker->reader,str,64);
	//printf("%s\n",str);
	appendToken(worker->tlist,str,-canextend);
	restartdfainstance(worker->dfa);
      }
      tokenizerFail(c);
    }
  }
}

void printTokenlistWithName(tokenizer *tokenizer)
{
  printf("\n");
  token *pos;
  for_each_token(pos,(tokenizer->tlist)){
    int gindex=pos->gindex;
    int sindex=pos->sindex;
    if(gindex >= tokenizer->gtable->bias){
      symbolitem *gitem=searchSymboltableById(tokenizer->gtable,gindex);
      printf("%s ",gitem->name);
    }
    else
      printf("TERMINAL ");
    symbolitem *sitem=searchSymboltableById(tokenizer->ttable,sindex);
    printf("%s\n",sitem->name);
    //printf("%d %d\n",gindex,sindex);
  }
}

#endif
