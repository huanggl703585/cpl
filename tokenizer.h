#ifndef __TOKENIZER_H
#define __TOKENIZER_H

#include "txtreader.h"
#include "mapinput.h"

//a tokenize shoule be hard-coding, because we just use one or two tokenizer

typedef struct tokenizer tokenizer;
struct tokenizer{
  reader *reader;
  charmapper *mapper;
};


#endif
