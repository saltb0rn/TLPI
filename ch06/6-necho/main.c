#define _GNU_SOURCE
#include <stdlib.h>
#include "tlpi_hdr.h"

#ifdef __GLIBC__
extern char* program_invocation_name;
extern char* program_invocation_short_name;
#endif

int
main(int argc, char* argv[]) {

  #ifdef __GLIBC__
  printf("program_invocation_name: %s\n", program_invocation_name);
  printf("program_invocation_short_name: %s\n", program_invocation_short_name);
  #endif

  #ifndef POINTER
  int j;
  for (j = 0; j < argc; j++)
    printf("argv[%d] = %s\n", j, argv[j]);
  #else
  char **p;
  for (p = argv; *p != NULL; p++)
    puts(*p);
  #endif

  return EXIT_SUCCESS;
}
