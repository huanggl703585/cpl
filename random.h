#ifndef __RANDOM_H
#define __RANDOM_H

#include <stdlib.h>
#include <limits.h>
#include <time.h>
/*
  character set equals to the ascii visiblie character set
*/
#define CHARACTER_MIN 32
#define CHARACTER_MAX 126

int seed;

/*
  a^b mod m
*/
int modexp(int a,int b,int n)
{
  if(a==0 || n==0)
    return -1;
  int ret=1;
  int i=0;
  while(b){
    if(b&0x1){
      ret=ret*a;
      ret=ret<<i;
      ret=ret%n;
    }
    b=b>>1;
    i++;
  }
  return ret;
}

/*
  p,q are both prime
  random number is between [0,p*q)
  base should between [2,p*q-1]
*/

struct RSArandomseed{
  int p;
  int q;
  int d;
  int base;
};

struct RSArandomseed stringlen={5,3,3,2};
struct RSArandomseed character={97,1,2};

int RSArandom(struct RSArandomseed seed)
{
  int ret;
  int n=seed.p*seed.q;
  ret=modexp(seed.base,seed.d,n);
  return ret;
}

int timerandom()
{
  seed=(seed+time(NULL))%INT_MAX;
  srand(seed);
  return rand();
}

/*
  return value [lower,upper)
*/
int getrandomnumber(int lower,int upper)
{
  int temp=timerandom();
  return (temp%(upper-lower)+lower);
}
#endif
