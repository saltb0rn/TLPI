#include <fcntl.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

/* Quote from the docs about write(2):

  `If the file was open(2)ed with O_APPEND, the file offset is  first  set  to
  the  end  of the file before writing.  The adjustment of the file offset
  and the write operation are performed as an atomic step.`

  So the answer to this exercise is the seek function doesn't affect file with flag O_APPEND.
 */

int main(int argc, char *argv[]) {
  if (argc != 3) {
    usageErr("[USAGE]: %s FILE CONTENT", argv[0]);
  }

  int fd;
  if ((fd = open(argv[1], O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR)) == -1) {
    errExit("open");
  }

  if (lseek(fd, 0, SEEK_SET) == -1) {
    errExit("lseek");
  }

  if (write(fd, argv[2], strlen(argv[2])) == -1) {
    errExit("write");
  }

  if (close(fd) == -1) {
    errExit("close");
  }

  return EXIT_SUCCESS;
}
