#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#ifndef STR_SIZ
#define STR_SIZ 100
#endif

int main(int argc, char *argv[]) {
  struct iovec iov[3];
  struct stat myStruct;         /* First buffer */
  int x;                        /* Second buffer */
  char str[STR_SIZ];           /* Third buffer */

  ssize_t numRead, totRequired;

  if (2 != argc || 0 == strcmp(argv[1], "--help")) {
    usageErr("%s file\n", argv[0]);
  }

  int fd = open(argv[1], O_RDONLY);
  if (-1 == fd) {
    errExit("open");
  }

  totRequired = 0;

  iov[0].iov_base = &myStruct;
  iov[0].iov_len = sizeof(myStruct);
  totRequired += iov[0].iov_len;

  iov[1].iov_base = &x;
  iov[1].iov_len = sizeof(x);
  totRequired += iov[1].iov_len;

  iov[2].iov_base = str;
  iov[2].iov_len = sizeof(str);
  totRequired += iov[2].iov_len;

  /* scatter read, read content into different parts */
  numRead = readv(fd, iov, 3);
  if (-1 == numRead) {
    errExit("readv");
  }

  if (numRead < totRequired) {
    printf("Read fewer bytes than requested\n");
  }

  printf("total bytes requested: %ld; bytes read: %ld\n",
         (long) totRequired, (long) numRead);

  /* printf("\nThe content from the first buffer: ============= \n"); */
  /* printf("myStruct: %s\n", (char *)iov[0].iov_base); */

  /* printf("\nThe content from the second buffer: ============= \n"); */
  /* printf("x: %s\n", (char *)iov[1].iov_base); */

  /* printf("\nThe content from the third buffer: ============= \n"); */
  /* printf("str: %s\n", (char *)iov[2].iov_base); */

  /* There is difference between printf and writev,
     writev will concat the data from iov, and printf doesn't, like above comments */
  writev(1, iov, 3);

  if (-1 == close(fd)) {
    errExit("close");
  }

  return EXIT_SUCCESS;
}
