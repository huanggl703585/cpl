#ifndef __PREPROCESS_H
#define __PREPROCESS_H

//preprocess for input txt
//1. delete annotation ( use # to note a line)
//2. find keyword section, and add to keyword dictionary

#include "txtreader.h"
#include "keyword.h"
#include "jumptable_link.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

kwdict *preprocess(char *filename,int *newfd);
jumptable_link *createpreprocesstable();

//newname is #filename
kwdict *preprocess(char *filename,int *newfd)
{
  reader *r=createreader(filename,4096);
  int len=strlen(filename);
  char newname[len+2];
  sprintf(newname,"#%s",filename);
  int dstfd=creat(newname,S_IRWXU);
 
  kwdict *ret=createkwdicthead();
 
  jumptable_link *table=createpreprocesstable();
  int state=1;
  int keywordpt=0,padpt=0;
  char keyword[64],pad[64];
  int isannotation=0;
  char c;
  while((c=readone(r))!='\0'){
    if(isannotation==0 && c=='#')
      isannotation=1;
    if(isannotation==1){
      if(c=='\n') isannotation=0;
      continue;
    }
    
    int prev=state;
    int next=jumptablefind(table,state,c);
    if(next==0)
      state=jumptablefind(table,state,0);
    else
      state=next;

    if(state==14){
      if(prev==1 || prev==14)
	write(dstfd,&c,1);
    }
    
    if(state==1)
      write(dstfd,"\n",1);
    
    if(state==13){
      if(prev!=13){
	bzero(keyword,64);
	keywordpt=0;
      }
      keyword[keywordpt++]=c;
    }
    if(state!=13 && prev==13){
      keyword[keywordpt++]='\0';
      kwdictinsert(ret,keyword);
    }   
  }
  return ret;
}

//we take 'other' as input 0
jumptable_link *createpreprocesstable()
{
  jumptable_link *ret=createjumptablelink();
  jumptableadd(ret,1,'\n',1);
  jumptableadd(ret,1,0,14);
  jumptableadd(ret,1,'k',3);

  jumptableadd(ret,3,'e',4);
  jumptableadd(ret,4,'y',5);
  jumptableadd(ret,5,'w',6);
  jumptableadd(ret,6,'o',7);
  jumptableadd(ret,7,'r',8);
  jumptableadd(ret,8,'d',9);
  jumptableadd(ret,3,0,14);
  jumptableadd(ret,4,0,14);
  jumptableadd(ret,5,0,14);
  jumptableadd(ret,6,0,14);
  jumptableadd(ret,7,0,14);
  jumptableadd(ret,8,0,14);

  jumptableadd(ret,9,' ',9);
  jumptableadd(ret,9,':',10);
  jumptableadd(ret,9,'\n',1);
  jumptableadd(ret,9,'\t',9);
  jumptableadd(ret,10,':',11);
  jumptableadd(ret,11,'=',12);
  jumptableadd(ret,9,0,14);
  jumptableadd(ret,10,0,14);
  jumptableadd(ret,11,0,14);

  jumptableadd(ret,12,' ',12);
  jumptableadd(ret,12,0,13);
  
  jumptableadd(ret,13,0,13);
  jumptableadd(ret,13,'|',12);
  jumptableadd(ret,13,'\n',9);

  jumptableadd(ret,14,0,14);
  jumptableadd(ret,14,'\n',1);
  return ret;
}
#endif
