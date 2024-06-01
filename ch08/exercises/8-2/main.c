#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "tlpi_hdr.h"

static void
show_help_info(char *cmdline, int status) {
  fprintf(status > 0 ? stderr: stdout, "Usage: %s USERNAME\n", cmdline);
  exit(status);
}

struct passwd*
_getpwnam(char *name) {
  struct passwd *p;
  long usernameMax;

  usernameMax = sysconf(_SC_LOGIN_NAME_MAX);
  if (-1 == usernameMax)
    usernameMax = 256;

  setpwent();                   /* start scanning from the beginning of the file */
  for (p = getpwent(); p != NULL && strncmp(p->pw_name, name, usernameMax) != 0; p = getpwent());
  endpwent();

  return p;
}

int
main(int argc, char *argv[]) {
  char *name;
  struct passwd *info;

  if (2 != argc)
    show_help_info(argv[0], EXIT_FAILURE);

  name = argv[1];
  errno = 0;                    /* errno must be set to zero to identify error scenario from non existent user */
  info = _getpwnam(name);

  if (NULL == info) {
    if (0 == errno) {
      printf("User %s does not exist in this system.\n", name);
      exit(EXIT_FAILURE);
    } else {
      errExit("_getpwnam");
    }
  }

  printf("User name: %s\n", info->pw_name);
  printf("Encrypted password: %s\n", info->pw_passwd);
  printf("User ID: %ld\n", (long) info->pw_uid);
  printf("Group ID: %ld\n", (long) info->pw_gid);
  printf("Comment: %s\n", info->pw_gecos);
  printf("Home directory: %s\n", info->pw_dir);
  printf("Login shell: %s\n", info->pw_shell);

  return EXIT_SUCCESS;
}
