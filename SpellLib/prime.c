#include <stdio.h>
#include <math.h>

int isprime(long i) {
long f, d, squirt;

  for (squirt = i; i/squirt < squirt; squirt >>= 1) /* Loop */;
  squirt = (squirt << 1) + 1;

  for (f = 3; f <= squirt; f += 2) {
    d = i / f;
    if (d*f == i) {
#ifdef DBMAIN
      fprintf(stderr, "%ld = %ld * %ld\n", i, f, d);
#endif
      return(0!=0);
    }
  }
  return(0==0);
}

long prime(long i) {
  if ((i&1) == 0) i++;
  for (;;) {
    if (isprime(i)) return(i);
    i += 2;
  }
}

#ifdef DBMAIN
int main(int argc, char **argv) {
long i, p;
  if (argc == 1) {
    fprintf(stderr, "%ld\nStarting number: ",32771L*32771L);
    fscanf(stdin, "%ld", &i);
  } else {
    sscanf(argv[1], "%ld", &i);
  }
  p = prime(i);
  fprintf(stderr, "The next prime up from %ld is %ld\n", i, p);
}
#endif
