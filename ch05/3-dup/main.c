/*
  The program implements cat FILE 2>&1.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZ
#define BUF_SIZ 1024
#endif

static void show_help_info(char *cmd_name, int status) {
  fprintf(status > 0 ? stderr: stdout, "Usage: %s FILE\n", cmd_name);
  exit(status);
}

int main(int argc, char *argv[]) {

  if (2 != argc) {
    show_help_info(argv[0], EXIT_FAILURE);
  }

  int fd = open(argv[1], O_RDONLY);
  if (-1 == fd) {
    errExit("open");
  }

  char buf[BUF_SIZ];
  ssize_t numRead;

  close(2);
  dup2(1, 2);
  while ((numRead = read(fd, buf, BUF_SIZ)) > 0) {
    if (-1 == write(1, buf, numRead)) {
      errExit("write");
    }
  }
  if (-1 == numRead) {
    errExit("read");
  }

  if (-1 == close(fd)) {
    errExit("close");
  }

  return EXIT_SUCCESS;
}
