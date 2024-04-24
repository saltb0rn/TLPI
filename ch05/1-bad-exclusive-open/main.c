/* This program demonstrates exclusive file creation how to affected multiprocess programming

   Compile without O_EXCL: make
   Compile with O_EXCL: make CFLAGS="-DEXCL"

   Then run the test script: ./test.sh
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZ
#define BUF_SIZ 1024
#endif

static void show_help_info(char *cmd_name, int status) {
  fprintf(status > 0 ? stderr: stdout, "Usage: %s FILE [OPT]\n", cmd_name);
  exit(status);
}

int main(int argc, char *argv[]) {

  if (2 > argc) {
    show_help_info(argv[0], EXIT_FAILURE);
  }

  int fd = open(argv[1], O_WRONLY);
  if (-1 != fd) {               /* Check if file exists */
    printf("[PID %ld] File \"%s\" already exists\n", (long) getpid(), argv[1]);
    close(fd);
  } else {
    if (ENOENT != errno) {      /* Failed for unexpected reason */
      errExit("open");
    } else {
      printf("[PID %ld] File \"%s\" doesn't exist yet\n", (long) getpid(), argv[1]);
      if (argc > 2) {
        sleep(2);
        printf("[PID %ld] Done sleeping\n", (long) getpid());
      }

      int flags = O_WRONLY | O_CREAT;
#ifdef EXCL
  flags |= O_EXCL;
#endif
      fd = open(argv[1], flags, S_IRUSR | S_IWUSR);
      if (-1 == fd) {
        errExit("open");
      }

      printf("[PID %ld] Created file \"%s\" exclusively\n", (long) getpid(), argv[1]);
    }
  }

  return EXIT_SUCCESS;
}
