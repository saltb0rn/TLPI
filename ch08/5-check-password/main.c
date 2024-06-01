#define _DEFAULT_SOURCE             /* Get getpass() declaration from <unistd.h> */
#define _XOPEN_SOURCE           /* Get crypt() declaration from <unistd.h> */
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[]) {
  char *username, *password, *encrypted, *p;
  struct passwd *pwd;
  struct spwd *spwd;
  Boolean authOk;
  size_t len;
  long lnmax;
  lnmax = sysconf(_SC_LOGIN_NAME_MAX);
  if (-1 == lnmax)
    lnmax = 256;

  username = malloc(lnmax);
  if (NULL == username)
    errExit("malloc");

  printf("Username: ");
  fflush(stdout);
  if (NULL == fgets(username, lnmax, stdin))
    exit(EXIT_FAILURE);         /* Exit on EOF */

  len = strlen(username);
  if ('\n' == username[len - 1])
    username[len - 1] = '\0';

  pwd = getpwnam(username);
  if (NULL == pwd)
    fatal("couldn't get password record");
  spwd = getspnam(username);
  if (NULL == spwd && EACCES == errno)
    fatal("no permission to read shadow password file");

  if (NULL != spwd)             /* If there is a shadow password record */
    pwd->pw_passwd = spwd->sp_pwdp; /* Use the shadow password */

  password = getpass("Password: ");
  /* Encrypt password and erase cleartext version immediately */

  encrypted = crypt(password, pwd->pw_passwd);
  for (p = password; *p != '\0';)
    *p++ = '\0';

  if (NULL == encrypted)
    errExit("crypt");

  authOk = strcmp(encrypted, pwd->pw_passwd) == 0;
  if (!authOk) {
    printf("Incorrect password\n");
    exit(EXIT_FAILURE);
  }

  printf("Successfully authenticated: UID=%ld\n", (long)pwd->pw_uid);
  free(username);

  return EXIT_SUCCESS;
}
