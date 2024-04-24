#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char* args[]) {
  int fd = open("startup", O_RDONLY);
  if (-1 == fd)
    errExit("open");
  close(fd);

  /* Open new or existing file for reading and writing, truncating to zero
     bytes; file permissions read+write for owner, nothing for all others
   */
  fd = open("myfile", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (-1 == fd)
    errExit("open");
  close(fd);

  /* Open new or existing file for writing; writes should always
     append to end of file.
   */
  fd = open("w.log", O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,
            S_IRUSR | S_IWUSR);
  if (-1 == fd)
    errExit("open");
  close(fd);

  return 0;
}
