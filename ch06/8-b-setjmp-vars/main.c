#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf env;

static void
doJump(int nvar, int rvar, int vvar)
{
  printf("Inside doJump(): nvar=%d rvar=%d vvar=%d\n", nvar, rvar, vvar);
  longjmp(env, 1);
}

int
main(int argc, char* argv[]) {

  int nvar;
  register int rvar;            /* Allocated in register if possible */
  volatile int vvar;

  nvar = 111;
  rvar = 222;
  vvar = 333;

  if (setjmp(env) == 0) {       /* Code executed after setjmp() */
    nvar = 777;
    rvar = 888;
    vvar = 999;
    doJump(nvar, rvar, vvar);
  } else {                      /* Code executed after longjmp() */
    printf("After longjmp(): nvar=%d rvar=%d vvar=%d\n", nvar, rvar, vvar);
  }

  return EXIT_SUCCESS;
}
