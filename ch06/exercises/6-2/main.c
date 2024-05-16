/*
  Compile in two different ways:
  1. make
  2. make CFLAGS="-O"

  In practice, the program generated from the 1st way could go back to the returned function normally.
  However, the program generated from the 2nd way will crash.  
 */

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static jmp_buf env;

void
installSetjmp() {
  switch(setjmp(env)) {
    case 0:
      printf("`setjmp` called, returning from function.\n");
      break;

    case 1:
      printf("Jump backed from other functions");
      break;
  }

  printf("Continuing function\n");
}

void
tryJmp() {
  longjmp(env, 1);
}

int
main(int argc, char* argv[]) {

  printf("Program started, start test.\n");
  installSetjmp();

  printf("Function has returned, now trying to jump back to it.\n");
  tryJmp();

  printf("Tried to perform jump, now back to main function. You might not always see this. Finishing.\n");
  return EXIT_SUCCESS;
}
