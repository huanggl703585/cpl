#include "dfa.h"
#include "re.h"
int main()
{
  re_seq *seq=initreseq(5,7);
  insertresymbol(seq,5);
  insertresymbol(seq,6);
  insertresymbol(seq,5);
  insertresymbol(seq,6);
  insertresymbol(seq,6);
  insertreoperator(seq,LEFTPARTH);
  insertreoperator(seq,OR);
  insertreoperator(seq,RIGHTPARTH);
  insertreoperator(seq,STAR);
  insertreoperator(seq,CAT);
  insertreoperator(seq,CAT);
  insertreoperator(seq,CAT);

  expandreseq(seq);
  re_iterator *iter=createreiterator(seq);

  re_node *ret=buildtree(seq);
  printf("\n");
  travelretree(ret);
  
  dfa* dfa=createdfa(ret,12);
  dfa_instance *instance=createdfainstance(dfa);
  printf("%d ",walkdfa(instance,5));
  printf("%d ",walkdfa(instance,6));
  printf("%d ",walkdfa(instance,5));
  printf("%d ",walkdfa(instance,6));
  
}
