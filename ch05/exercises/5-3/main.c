/*
  ./atomic_append f1 10000 & ./atomic_append f1 10000
  ./atomic_append f2 10000 x & ./atomic_append f2 10000 x

  We can exec "ls -l" to tell the difference between two commands above.

  Something you need to know, CMD1 & CMD2: First exec CMD1 in background, then exec CMD2
 */

#include <stdlib.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef WRITE
#define WRITE(fd,buf,count) do {                \
    if (useAppend) {                            \
      lseek(fd, 0, SEEK_END);                   \
    }                                           \
    write(fd, buf, count);                      \
  } while(0)
#endif

#ifndef PADDING
#define PADDING ','
#endif

int main (int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    errExit("[USAGE]: %s filename num-bytes [x]\n");
  }

  int flags = O_WRONLY | O_CREAT;
  Boolean useAppend = argc == 3 ? TRUE: FALSE;

  if (useAppend) {
    flags |= O_APPEND;
  }

  int fd;
  if ((fd = open(argv[1], flags)) == -1) {
    errExit("open");
  }

  int numBytes = atoll(argv[2]);
  int timesForW4BPerTime = numBytes >> 2;
  int bytesRest = numBytes & 3;

  while (timesForW4BPerTime--) {
    WRITE(fd, (char [4]){ PADDING }, 4);
  }
  while (bytesRest--) {
    WRITE(fd, (char []){ PADDING }, 1);
  }

  if (close(fd) == -1) {
    errExit("close");
  }

  return EXIT_FAILURE;
}
