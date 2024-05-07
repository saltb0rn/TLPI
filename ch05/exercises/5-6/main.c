#include <fcntl.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  if (2 != argc) {
    usageErr("Usage: %s FILE\n", argv[0]);
  }

  int fd1 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (-1 == fd1) {
    errExit("open");
  }

  int fd2 = dup(fd1);
  if (-1 == fd2) {
    errExit("dup");
  }

  int fd3 = open(argv[1], O_RDWR);
  if (-1 == fd3) {
    errExit("open");
  }

  /* fd1 and fd2 share same offset */
  if (-1 == write(fd1, "Hello, ", 7))
    errExit("write");

  if (-1 == write(fd2, "world!", 7))
    errExit("write");
  /* So the content of FILE will be "Hello, world!" */

  /* Reset offset */
  if (-1 == lseek(fd2, 0, SEEK_SET))
    errExit("lseek");

  /* Write "Hello, " to FILE with fd1 at offset 0 */
  if (-1 == write(fd1, "HELLO, ", 7))
    errExit("write");

  /* Write "Gidday" to FILE with fd3 at offset 0,
     so the write function called by fd1 doesn't affect this */
  if (-1 == write(fd3, "Gidday", 6))
    errExit("write");

  if (-1 == close(fd1))
    errExit("close");

  if (-1 == close(fd2))
    errExit("close");

  if (-1 == close(fd3))
    errExit("close");

  return EXIT_SUCCESS;
}
