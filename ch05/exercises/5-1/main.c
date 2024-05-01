#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

static void show_help_info(char *cmd_name, int status) {
  fprintf(status > 0 ? stderr: stdout, "Usage: %s FILE [OFFSET]\n", cmd_name);
  exit(status);
}

int main(int argc, char *argv[]) {
  if (argc != 2 && argc != 3) {
    show_help_info(argv[0], EXIT_FAILURE);
  }
  int fd = open(argv[1],
                O_WRONLY | O_CREAT | O_SYNC,
                S_IRUSR | S_IWUSR |
                S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);
  int flags, accessMode;

  flags = fcntl(fd, F_GETFL);
  if (-1 == flags) {
    errExit("fcntl");
  }

  /* get status flags */
  if (flags & O_SYNC) {
    printf("writes are synchronized\n");
  }

  /* get flags that stand for access mode: read/write/read&write */
  accessMode = flags & O_ACCMODE;
  if (O_WRONLY == accessMode || O_RDWR == accessMode) {
    printf("file is writable\n");
  }

  /* set flags */
#ifndef _FILE_OFFSET_BITS
  flags |= O_APPEND;
#endif
  if (-1 == fcntl(fd, F_SETFL, flags)) {
    errExit("fcntl");
  }

  if (argc == 3) {
    off_t offset = atoll(argv[2]);

    if (-1 == lseek(fd, offset, SEEK_SET)) {
      errExit("lseek");
    }

    if (-1 == write(fd, "", 1)) {
      errExit("write");
    }
  }

  if (close(fd) == -1) {
    errExit("close");
  }

  return EXIT_SUCCESS;
}
