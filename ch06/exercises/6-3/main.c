#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

extern char** environ;

#ifndef BUF_SIZ
#define BUF_SIZ 1024
#endif

/*
  getEnvValue and getEnvName cloud actually be implemented via regular expression
*/

/*
  char *env: a string looks like "ENV=VALUE"
  char *buf: a char array to store the "VALUE"

  return -1 for invalid string, 0 for valid string.
 */
int
getEnvValue(char *env, char *buf) {
  char *p;
  char i;

  p = env;
  /* move the pointer to '=' */
  while (*p != '=' && *p != '\0') ++p;
  if (*p == '\0') return -1;

  /* save the string that follows '=' */
  for (i = 0, ++p; *p != '\0'; ++i, ++p) {
    buf[i] = *p;
  }

  buf[i] = '\0';

  return 0;
}

/*
  char *env: a string looks like "ENV=VALUE"
  char *buf: a char array to store the "ENV"

  return -1 for invalid string, 0 for valid string.
 */
int
getEnvName(char *env, char *buf) {
  char *p;
  int i;

  for (i = 0, p = env; *p != '=' && *p != '\0'; ++i, ++p) {
    buf[i] = *p;
  }

  buf[i] = '\0';

  return *p == '=' ? 0: -1;
}

int
_setenv(const char *name, const char *value, int overwrite) {

  if (name == NULL || *name == '\0' || *name == '=') {
    errno = EINVAL;
    return -1;
  }

  int envLength;
  char *envStr;
  long argMax;

  argMax = sysconf(_SC_ARG_MAX);
  envLength = strlen(name) + strlen(value) + 2; /* name + '=' + value + '\0' */

  if ((long) envLength >= argMax) {
    errno = ENOMEM;
    return -1;
  }

  if (getenv(name) != NULL && overwrite) {
    return 0;
  }

  envStr = malloc(envLength);
  if (NULL == envStr) {
    errExit("malloc");
  }

  if (sprintf(envStr, "%s=%s", name, value) == -1) {
    errExit("sprintf");
  }

  return putenv(envStr);

}

int
_unsetenv(const char *name)
{
  char **ep,                    /* the environment pointer */
       **epIdx;                 /* iterator for shifting environment data */
  char _name[BUF_SIZ];

  if (name == NULL || *name == '\0' || *name == '=') {
    errno = EINVAL;
    return -1;
  }

  for (ep = environ; *ep != NULL; ++ep) {
    if (getEnvName(*ep, _name) == -1) {
      return -1;
    }

    if (strncmp(name, _name, BUF_SIZ) == 0) {
      /* shift all subsequent env vars in order to remove the current one */
      for (epIdx = ep; *epIdx != NULL; ++epIdx) {
        *epIdx = *(epIdx + 1);
      }
    }
  }

  return 0;
}

static void
show_help_info(char* cmd_name, int status) {
  fprintf(status > 0 ? stderr: stdout, "Usage: %s [-s NAME=VALUE] [-u NAME] [-g NAME]\n", cmd_name);
  exit(status);
}

static void
printenv() {
  char **ep = environ;
  for (; *ep != NULL; ep++) {
    printf("%s\n", *ep);
  }
}

int
main(int argc, char* argv[]) {

  if (argc < 2) {
    show_help_info(argv[0], EXIT_FAILURE);
  }

  int opt;
  char name[BUF_SIZ], value[BUF_SIZ];
  char *readValue;

  // opterr = 0;
  while ((opt = getopt(argc, argv, "+s:u:g:")) != -1) {
    switch(opt) {
      case 's':
        if (getEnvName(optarg, name) == -1 ||
            getEnvValue(optarg, value) == -1) {
          errExit("Invalid format: %s\n", optarg);
        }

        if (_setenv(name, value, 1) == -1) {
          printf("Failed to set env var %s\n", name);
          errExit("_unsetenv");
        } else {
          printf("Env var %s set to %s\n\n", name, value);
          printenv();
        }
        break;

      case 'u':
        if (_unsetenv(optarg) == -1) {
          printf("Failed to unset env var %s\n", optarg);
          errExit("_unsetenv");
        } else {
          printf("Env var %s unset\n", optarg);
          printenv();
        }
        break;

      case 'g':
        readValue = getenv(optarg);
        if (readValue == NULL) {
          printf("Env var %s is not set\n", optarg);
        } else {
          printf("Env var %s value: %s\n", optarg, readValue);
        }
        break;

      case '?':
        show_help_info(argv[0], EXIT_SUCCESS);
        break;
    }
  }

  return EXIT_SUCCESS;
}
