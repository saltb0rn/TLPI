#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZ
#define BUF_SIZ 1024
#endif

#ifndef MAX_OUT_FILES
#define MAX_OUT_FILES 128
#endif

static void show_help_info(char* cmd_name, int status) {
  fprintf(status > 0 ? stderr: stdout, "Usage: %s [-a] FILE ...\n", cmd_name);
  exit(status);
}

int main(int argc, char *argv[]) {
  int opt;
  Boolean flag_append = FALSE;
  while (-1 != (opt = getopt(argc, argv, "ha"))) {
    switch(opt) {
      case '?':                 /* unrecognized options */
        show_help_info(argv[0], EXIT_FAILURE);
        break;
      case 'a':
        flag_append = TRUE;
        printf("%d\n", optind);
        break;
      case 'h':
        printf("%d\n", optind);
        show_help_info(argv[0], EXIT_SUCCESS);
        break;
    }
  }

  /* This makes sure argument FILE are passed.

     Let me explained:

     argc must be larger or equal to 1,

     when call ./command, argc is 1, optind is 1.

     When call ./command -a, argc is 2, after options are all processed, optind is 2.

     These situations above are that arguments FILEs are missing.

     When ./command file, argc is 2, optind is 1.

     When ./command -a file, argc is 3, optind is 2.
   */
  if (optind >= argc) {
    show_help_info(argv[0], EXIT_FAILURE);
  }

  int flags = O_CREAT | O_WRONLY;
  mode_t modes = S_IRUSR | S_IWUSR;

  if (flag_append)
    flags |= O_APPEND;
  else
    flags |= O_TRUNC;

  int fds[MAX_OUT_FILES];
  char buf[BUF_SIZ + 1];
  int fcnt = 0;

  /* Process the arguments FILEs */
  for (int i = optind; i < argc; i++) {
    int fd = open(argv[i], flags, modes);
    fds[i - optind] = fd;
    if (-1 == fd) {
      errExit("open");
    }
    fcnt++;
  }

  /* write content from stdin to stdout and FILEs */
  ssize_t numRead = 0;
  while ((numRead = read(STDIN_FILENO, buf, BUF_SIZ)) > 0) {
    if (numRead != write(STDOUT_FILENO, buf, numRead)) {
      errExit("write");
    }

    for (int i = 0; i < fcnt; i++) {
      if (numRead != write(fds[i], buf, numRead)) {
        errExit("write");
      }
    }
  }
  if (-1 == numRead)            /* read(STDIN_FILENO, buf, BUF_SIZ) may raise error */
    errExit("open");

  /* close all FILEs */
  for (int i = 0; i < fcnt; i++) {
    if (-1 == close(fds[i]))
      errExit("close");
  }

  return EXIT_SUCCESS;
}
