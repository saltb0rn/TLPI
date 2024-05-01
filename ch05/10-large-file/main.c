#define _LARGEFILE64_SOURCE
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"


int main(int argc, char *argv[]) {
  int fd;
  off64_t off;
  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("Usage: %s PATHNAME OFFSET\n", argv[0]);
  }

  fd = open64(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (-1 == fd) {
    errExit("open64");
  }

  off = atoll(argv[2]);
  if (lseek64(fd, off, SEEK_SET) == -1) {
    errExit("lseek64");
  }

  if (write(fd, "test", 4) == -1) {
    errExit("write");
  }

  return EXIT_SUCCESS;
}
