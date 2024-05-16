#include <stdio.h>
#include <stdlib.h>

char globBuf[65536];            /* Uninitialized data segment */
int primes[] = {2, 3, 5, 7 };   /* Initialized data segment */

/*
  Solution for exercise 6-1:
  
  "globBuf" is destined to the Uninitialized data segment (bss).

   The system will initialize the data segment with zeros before starting the program.

   So, the executable file itself just needs to record the address and size of the "globBuf",

   which is then actually allocated by the program loader at run time.
 */

static int
square(int x)                   /* Allocated in frame for square() */
{
  int result;                   /* Allocated in frame for square() */

  result = x * x;
  return result;                /* Return value passed via register */
}

static void
doCalc(int val)                 /* Allocated in frame for doCalc() */
{
  printf("The square of %d is %d\n", val, square(val));

  if (val < 1000) {
    int t;                      /* Allocated in frame for doCalc() */

    t = val * val * val;
    printf("The cube of %d is %d\n", val, t);
  }
}

int
main(int argc, char *args[]) { /* Allocated in frame for main() */
  static int key = 9973;        /* Initialized data segment */
  static char mbuf[10240000];   /* Unintialized data segment */
  char *p;                      /* Allocated in frame for main() */

  p = malloc(1024);             /* Points to memory in heap segment */

  doCalc(key);
  free(p);

  return EXIT_SUCCESS;
}
