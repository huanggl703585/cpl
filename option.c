#ifndef __OPTION_C
#define __OPTION_C

//mark the option of the program
typedef struct option option;
struct option{

};

#define init_option(name)			\
  option name;					\
  bzero(name,sizeof(option))

int checkinput(option option,char *input[]);
void execute(option option,char *input[]);
#endif
