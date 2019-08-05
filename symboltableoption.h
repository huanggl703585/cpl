#ifndef __SYMBOLTABLE_OPTION_H
#define __SYMBOLTABLE_OPTION_H

typedef struct symboltableoption symboltableoption;
struct symboltableoption{
  int duplicate;
};

#define init_symboltable_option(symoption)	\
  symoption.duplicate=1;

#endif
