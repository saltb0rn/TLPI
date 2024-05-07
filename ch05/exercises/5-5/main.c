/*
  ./main FILE1 [FILE2]

  Create a file descriptor that refers to the previous descriptor.

  ./main FILE1 => {
      int newfd = dup(fd-of-FILE1);
      write(newfd, ...); // redirect to FILE1
  }

  ./main FILE1 FILE2 => {
      int newfd = dup2(fd-of-FILE1, fd-of-FILE2);
      write(newfd, ...); // redirect to FILE1
  }
 */

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "tlpi_hdr.h"

static int _dup(int fd) {
  int newfd = fcntl(fd, F_DUPFD, 0);
  return newfd;
}

static int _dup2(int oldfd, int newfd) {

  int flags = fcntl(oldfd, F_GETFL);
  if (-1 == flags) {
    errno = EBADF;
    return -1;
  }

  if (oldfd == newfd) {
    return newfd;
  }

  flags = fcntl(newfd, F_GETFL);
  if (-1 != flags) {            /* silently close file descriptor */
    close(newfd);
  }

  int dupfd = fcntl(oldfd, F_DUPFD, newfd);
  if (newfd != dupfd) {
    errMsg("_dup2 is not atomic, so sometimes it fails. It just did. Expected fd %d, got %d\n",
           newfd, dupfd);
  }

  return newfd;
}

static void show_help_info(char *cmdline) {
  usageErr("USAGE: %s FILE1 [FILE2]", cmdline);
}

int main(int argc, char *argv[]) {
  int fd, newfd = -1;

  if (argc < 2 || argc > 3) {
    show_help_info(argv[0]);
  }

  fd = open(argv[1], O_WRONLY);
  if (-1 == fd) {
    errExit("open");
  }

  if (3 == argc) {
    newfd = open(argv[2], O_RDWR | O_CREAT,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  }

  if (3 == argc) {
    newfd = _dup2(fd, newfd);
  } else {
    newfd = _dup(fd);
  }

  const char contStr[] = "Writing to the copied file description\n";
  int numWriten;

  if ((numWriten = write(newfd, contStr, sizeof(contStr))) == -1) {
    errExit("write");
  }

  off_t off1, off2;
  if ((off1 = lseek(fd, 0, SEEK_CUR)) == -1) {
    errExit("lseek");
  }
  if ((off2 = lseek(newfd, 0, SEEK_CUR)) == -1) {
    errExit("lseek");
  }

  assert(off1 == off2);
  printf("They share same offset: %ld\n", off1);

  int flags1, flags2;
  flags1 = fcntl(fd, F_GETFL);
  flags2 = fcntl(newfd, F_GETFL);

  assert(flags1 == flags2);
  printf("They share same flags\n");

  if (-1 == close(fd)) {
    errExit("close");
  }

  if (-1 == close(newfd)) {
    errExit("close");
  }

  printf("Done. Written %d bytes to the new file descriptor #%d\n",
         numWriten, newfd);

  return EXIT_SUCCESS;
}
