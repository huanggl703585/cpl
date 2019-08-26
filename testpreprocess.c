#include "preprocess.h"

int main()
{
  char *path="cgrammer.txt";

  int fd;
  kwdict *dict=preprocess(path,&fd);
  printkwdict(dict);
}
