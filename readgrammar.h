#ifndef __READGRAMMAR_H
#define __READGRAMMAR_H

#include "dfa.h"
#include "txtreader.h"
#include "mapinput.h"

/*
  ' ' '/n' '\0' : 1
  ':'           : 2
  '='           : 3
  '_'           : 4
  alpha         : 5
  digit         : 6
*/

re_seq *re_grammar;

void initreadgrammar()
{
  re_grammar=initreseq(9,15);
  // re_grammar=initreseq(5,9);
  insertresymbol(re_grammar,4);
  insertresymbol(re_grammar,5);
  insertresymbol(re_grammar,4);
  insertresymbol(re_grammar,5);
  insertresymbol(re_grammar,6);
  insertresymbol(re_grammar,2);
  insertresymbol(re_grammar,3);
  insertresymbol(re_grammar,3);
  insertresymbol(re_grammar,1);
  insertreoperator(re_grammar,LEFTPARTH);
  insertreoperator(re_grammar,OR);
  insertreoperator(re_grammar,RIGHTPARTH);
  insertreoperator(re_grammar,CAT);
  insertreoperator(re_grammar,LEFTPARTH);
  insertreoperator(re_grammar,OR);
  insertreoperator(re_grammar,OR);
  insertreoperator(re_grammar,RIGHTPARTH);
  insertreoperator(re_grammar,STAR);
  insertreoperator(re_grammar,OR);
  insertreoperator(re_grammar,LEFTPARTH);
  insertreoperator(re_grammar,CAT);
  insertreoperator(re_grammar,CAT);
  insertreoperator(re_grammar,RIGHTPARTH);
  insertreoperator(re_grammar,OR);
  expandreseq(re_grammar);
}

#endif
