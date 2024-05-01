#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZ
#define BUF_SIZ 128
#endif

int main(int argc, char *argv[]) {
  int fd;
  char template[] = "/tmp/somestringXXXXXX";
  fd = mkstemp(template);

  if (-1 == fd) {
    errExit("mkstemp");
  }
  printf("Generated filename was: %s\n", template);
  unlink(template);             /* Name disappears immediately, but the file
                                   is removed only after close() */
  /* Use file I/O system calls - read(), write(), and so on */

  char buf[BUF_SIZ];
  printf("Please text msg to tempfile: \n");
  fgets(buf, BUF_SIZ, stdin);

  if (-1 == write(fd, buf, BUF_SIZ)) {
    errExit("write");
  }

  if (-1 == read(fd, buf, BUF_SIZ)) {
    errExit("read");
  }
  printf("Content read from temp file: %s\n", buf);

  if (-1 == close(fd)) {
    errExit("close");
  }
  return EXIT_SUCCESS;
}
