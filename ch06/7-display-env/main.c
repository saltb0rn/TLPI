#define _GNU_SOURCE
#include <stdlib.h>
#include "tlpi_hdr.h"

#ifdef EXTERN
extern char** environ;
#endif

int
main(int argc, char* argv[]
     #ifndef EXTERN
     , char** envs
     #endif
     ) {

  char **ep;

  #ifdef EXTERN
  printf("Enviroments from external\n\n");
  for (ep = environ; *ep != NULL; ep++)
    puts(*ep);
  #else
  printf("Enviroments from arguments\n\n");
  for(ep = envs; *ep != NULL; ep++)
    puts(*ep);
  #endif

  return EXIT_SUCCESS;
}
