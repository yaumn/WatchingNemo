#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "aquarium.h"
#include "log.h"
#include "parser.h"
#include "tools.h"
#include "view.h"
#include "fish.h"


struct aquarium aquarium;
int aquarium_loaded = 0;


void aquarium_init(int width, int height)
{
  aquarium.size.width = width;
  aquarium.size.height = height;
  
  views_init();
  fishs_init();
  mobility_init(&aquarium);
  aquarium_loaded = 1;
}


int aquarium_load(char *aquarium_name)
{
  if (aquarium_loaded) {
    fprintf(stderr, "An aquarium is already loaded!\n");
    return 0;
  }
  
  char cwd[PATH_MAX];
  
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd");
    return 0;
  }
  
  strncat(cwd, "/", sizeof(cwd) - 1);
  strncat(cwd, aquarium_name, sizeof(cwd) - 1);

  const int success = parse_aquarium_file(cwd);

  log_write(1, success ? "%s loaded successfully" : "Failed to load %s",
	    aquarium_name);
  
  return success;
}


// Save the aquarium in a log file
int aquarium_save(char *aquarium_name)
{
  if (!aquarium_loaded) {
    fputs("No aquarium loaded\n", stderr);
    return 0;
  }
  
  char cwd[PATH_MAX];

  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd() error");
    return 0;
  }

  strncat(cwd, "/", sizeof(cwd)-1);
  strncat(cwd, aquarium_name, sizeof(cwd)-1);
  //printf("Aquarium path : %s\n", cwd);

  FILE *f = fopen(cwd, "w");
  if (f != NULL) {
    fputs("", f);
  }

  const struct size size = aquarium_get_size();
  
  fprintf(f, "%dx%d\n", size.width, size.height);
  views_save(f);
  //aquarium_finalize();
  fclose(f);

  log_write(1, "%s saved successfully", aquarium_name);
  
  return 1;
}


void aquarium_show()
{
  if (!aquarium_loaded) {
    fputs("No aquarium loaded\n", stderr);
    return;
  }

  printf("%dx%d\n", aquarium.size.width, aquarium.size.height);
  views_print();
}


void aquarium_finalize()
{
  if (!aquarium_loaded) {
    return;
  }
  
  mobility_finalize();
  fishs_finalize();
  views_finalize();
  aquarium_loaded = 0;
}

struct size aquarium_get_size()
{
  return aquarium.size;
}


int aquarium_is_loaded()
{
  return aquarium_loaded;
}
