#include <stdio.h>
#include <stdlib.h>
#include "ugid_functions.h"

static void
show_help_info(char *cmdname, int status) {
  fprintf(status > 0 ? stderr: stdout, "Usage: %s NAME\n", cmdname);
  exit(status);
}

int
main(int argc, char *argv[]) {

  if (argc != 2) {
    show_help_info(argv[0], EXIT_FAILURE);
  }

  uid_t uid = userIdFromName(argv[1]);
  char *username = userNameFromId(uid);
  printf("get user ID: %d from user name: %s\n", uid, username);
  gid_t gid = groupIdFromName(argv[1]);
  char *groupname = groupNameFromId(gid);
  printf("get group ID: %d from group name: %s\n", gid, groupname);

  return EXIT_SUCCESS;
}
