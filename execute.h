#ifndef __EXECUTE_H
#define __EXECUTE_H

void init();
void execute(char *input[]);

void init()
{
  buildmetagrammar();
}

void execute(char *input[])
{
  tokenlist *lgt=dotokenize(metalexcialgrammar,input[0]);
  tokenlist *sgt=dotokenize(metalexcialgrammar,input[2]);
  
  grammar *lg=doparse(metastructualgrammar,lgt);
  grammar *sg=doparse(metastructualgrammar,sgt);

  tokenlist *t=dotokenize(lg,input[1]);
  ast *root=doparse(sg,t);
}
#endif
