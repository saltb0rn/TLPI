#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#ifndef BUF_SIZ
#define BUF_SIZ 1024
#endif

int main(int argc, char *argv[]) {

  if (2 == argc) {

    int fd = open(argv[1], O_RDONLY);
    if (-1 == fd) {
      return EXIT_FAILURE;
    }

    char buf[BUF_SIZ];
    char zeros[BUF_SIZ];
    int lineNum = 0;
    ssize_t numRead;

    while ((numRead = read(fd, buf, BUF_SIZ)) > 0) {
      printf("LN: %d: %s\n", ++lineNum, buf);
    }
    /* CANNOT display content after file holes */

    if (-1 == close(fd)) {
      return EXIT_FAILURE;
    }

    goto SUCCESS;
  }

  int fd = open(argv[1],
                O_CREAT | O_TRUNC | O_WRONLY,
                S_IRUSR | S_IWUSR |
                S_IRGRP | S_IWUSR |
                S_IROTH | S_IWOTH);

  if (-1 == fd) {
    return EXIT_FAILURE;
  }

  const char digits[] = "0123456789";
  if (-1 == (write(fd, digits, (size_t)(sizeof(digits) / sizeof(char))))) {
    return EXIT_FAILURE;
  }

  lseek(fd, BUF_SIZ, SEEK_CUR);

  const char alpha[] = "abcdefghijklmnopqrstuvwxyz";
  if (-1 == write(fd, alpha, (size_t)(sizeof(alpha) / sizeof(char)))) {
    return EXIT_FAILURE;
  }

  if (-1 == close(fd)) {
    return EXIT_FAILURE;
  }

 SUCCESS:
  return EXIT_SUCCESS;
}
