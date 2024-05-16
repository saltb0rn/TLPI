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

  int j;
  char **ep;

  clearenv();                   /* Erase entire environment */

  for (j = 1; j < argc; j++)
    if (putenv(argv[j]) != 0)
      errExit("putenv: %s", argv[j]);

  if (setenv("GREET", "Hello world", 0) == -1)
    errExit("setenv");

  unsetenv("Bye");

  #ifdef EXTERN
  printf("Enviroments from external\n\n");
  for (ep = environ; *ep != NULL; ep++)
    puts(*ep);
  #else
  printf("Enviroments from arguments\n\n");
  printf("And the modifications won't show up here");
  for(ep = envs; *ep != NULL; ep++)
    puts(*ep);
  #endif

  return EXIT_SUCCESS;
}
