#ifndef __SCANNER_H
#define __SCANNER_H

#define bufsize 1024
typedef struct scanner scanner;
struct scanner{
  char buf1[bufsize];
  char buf2[bufsize];
  int rfpf;           //readfile pointer
};


#endif
