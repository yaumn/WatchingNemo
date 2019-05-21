#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "prompt_listener.h"


int main(int argc, char *argv[])
{
  if (argc == 2 || (argc > 1 && strcmp(argv[1], "-l") != 0) || argc > 3) {
    fprintf(stderr, "Usage: %s [-l verbosity_level]\n", argv[0]);
    return EXIT_FAILURE;
  }

  int log = 0;
  int level;
  if (argc == 3 && strcmp(argv[1], "-l") == 0) {
    level = strtol(argv[2], (char **)NULL, 10);
    if (errno == EINVAL || errno == ERANGE || level < 0 || level > 2) { 
      fputs("Verbosity level must be an integer between 0 and 2\n", stderr);
      return EXIT_FAILURE;
    }
    log = 1;
  }

  if (log && level > 0) {
    log_init("controller.log", level);
  }
  
  create_prompt_listener();

  if (log) {
    log_finalize();
  }
  
  return EXIT_SUCCESS;
}
