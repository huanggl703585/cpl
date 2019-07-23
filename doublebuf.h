#ifndef __DOUBLE_BUF_H
#define __DOUBLE_BUF_H

#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct doublebuf doublebuf;
struct doublebuf{
  int bufsize;
  int (*fillbuf)(char*,void*);
  int forward;
  int lexeme;
  char buf[];
};

#define fillfirstbuf(dbuf,arg) (dbuf->fillbuf(dbuf->buf,arg))
#define fillsecondbuf(dbuf,arg) (dbuf->fillbuf(dbuf->buf+dbuf->bufsize+1,arg))

#define cleanfirstbuf(dbuf) (bzero(dbuf->buf,dbuf->bufsize))
#define cleansecondbuf(dbuf) (bzero(dbuf->buf+dbuf->bufsize+1,dbuf->bufsize))

#define reachfirstend(dbuf) (dbuf->forward==dbuf->bufsize)
#define reachsecondend(dbuf) (dbuf->forward==dbuf->bufsize*2+1)

//arg is used to fillbuf
doublebuf* createdoublebuf(int bufsize,int (*fillbuf)(char*,void*),void* arg);
char readchar(doublebuf *dbuf,void *fillarg);
int readword(doublebuf *dbuf,char *recvbuf,int readsize);

doublebuf* createdoublebuf(int bufsize,int (*fillbuf)(char*,void*),void *arg)
{
  doublebuf* ret=(doublebuf*)malloc(sizeof(doublebuf)+2*bufsize+2);
  ret->bufsize=bufsize;
  ret->fillbuf=fillbuf;
  ret->forward=0;
  ret->lexeme=0;
  bzero(ret->buf,bufsize*2+2);
  ret->buf[bufsize]='\0';
  ret->buf[bufsize*2+1]='\0';
  if(fillfirstbuf(ret,arg))
    return NULL;
  return ret;
}

char readchar(doublebuf *dbuf,void *fillarg)
{
  char ret=dbuf->buf[dbuf->forward];
  
  if(ret=='\0'){
    if(reachfirstend(dbuf)){
      if(fillsecondbuf(dbuf,fillarg))
	return 0;
      ret=dbuf->buf[++(dbuf->forward)];
      cleanfirstbuf(dbuf);
    }
    else if(reachsecondend(dbuf)){
      if(fillfirstbuf(dbuf,fillarg))
	return 0;
      dbuf->forward=0;
      ret=dbuf->buf[dbuf->forward];
      cleansecondbuf(dbuf);
    }
  }
  dbuf->forward++;
  return ret;
}

int readword(doublebuf *dbuf,char *recvbuf,int readsize)
{
  int lexlen;
  if((lexlen=dbuf->forward-dbuf->lexeme-1)>=0){
    if(readsize<lexlen) return -1;
    memcpy(recvbuf,dbuf->buf+dbuf->lexeme,lexlen);
  }
  else{
    int firstpart=2*(dbuf->bufsize)+2-dbuf->lexeme;
    int secondpart=dbuf->forward-1;
    lexlen=firstpart+secondpart;
    if(readsize<lexlen) return -1;
    memcpy(recvbuf,dbuf->buf+dbuf->lexeme,firstpart);
    memcpy(recvbuf+firstpart,dbuf->buf,secondpart);
  }
  dbuf->lexeme=dbuf->forward;
  return lexlen;
}
#endif
