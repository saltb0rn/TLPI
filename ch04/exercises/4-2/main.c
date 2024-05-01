#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZ
#define BUF_SIZ 1024
#endif

static void show_help_info(char *cmd_name, int status) {
  fprintf(status > 0 ? stderr: stdout, "Usage: %s FILE1 FILE2\n", cmd_name);
  exit(status);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    show_help_info(argv[0], EXIT_FAILURE);
  }

  char *srcFile = argv[1],
    *dstFile = argv[2];

  int srcFlags = O_RDONLY,
    dstFlags = O_WRONLY | O_CREAT | O_TRUNC;
  mode_t dstModes = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  int srcFd, dstFd;
  if (-1 == (srcFd = open(srcFile, srcFlags))) {
    errExit("open");
  }

  if (-1 == (dstFd = open(dstFile, dstFlags, dstModes))) {
    errExit("open");
  }

  ssize_t numRead;
  char buf[BUF_SIZ];
  char zeros[BUF_SIZ];

  while ((numRead = read(srcFd, buf, BUF_SIZ)) > 0) {
    if (-1 == numRead) {
      errExit("read");
    }

    if (0 == memcmp(buf, zeros, numRead)) {
      /* holes found, skip */
      lseek(dstFd, numRead, SEEK_CUR);
    } else {
      if (numRead != write(dstFd, buf, numRead)) {
        errExit("write");
      }
    }
  }

  if (-1 == close(srcFd)) {
    errExit("close");
  }

  if (-1 == close(dstFd)) {
    errExit("close");
  }

  return EXIT_SUCCESS;
}
