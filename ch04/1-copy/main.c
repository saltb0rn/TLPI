#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[]) {
  int inputFd, outputFd, openFlags;
  mode_t filePerms;
  ssize_t numRead;
  char buf[BUF_SIZE];

  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s old-file new-file\n", argv[0]);

  /* Open input and output files */
  inputFd = open(argv[1], O_RDONLY);
  if (-1 == inputFd)
    errExit("opening file %s", argv[2]);

  openFlags = O_CREAT | O_WRONLY | O_TRUNC;
  filePerms =
    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw-rw-rw- */

  outputFd = open(argv[2], openFlags, filePerms);
  if (-1 == outputFd)
    errExit("opening file %s", argv[2]);

  /* Transfer data until we encounter end of input or an error */
  while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
    if (write(outputFd, buf, numRead) != numRead)
      fatal("couldn't write whole buffer");

  if (numRead == -1)
    errExit("read");

  if (-1 == close(inputFd))
    errExit("close input");
  if (-1 == close(outputFd))
    errExit("close output");

  exit(EXIT_SUCCESS);
}
