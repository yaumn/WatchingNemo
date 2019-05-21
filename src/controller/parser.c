#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aquarium.h"
#include "parser.h"
#include "tools.h"
#include "view.h"


#define MAX_PARAM_LENGTH 32


int parse_config_file(const char *filename, struct controller_config *config)
{
  FILE *fp = NULL;
  char *line = NULL;
  size_t len = 0;
  
  if ((fp = fopen(filename, "r")) == NULL) {
    perror("fopen");
    return 0;
  }

  while (getline(&line, &len, fp) != -1) {
    if (line[0] == '#') {
      continue;
    }

    char name[MAX_PARAM_LENGTH];
    char value[MAX_PARAM_LENGTH];
    
    if (sscanf(line, "%s = %s", name, value) == 2) {
      if (strcmp(name, "controller-port") == 0) {
	config->port = atoi(value);
      } else if (strcmp(name, "display-timeout-value") == 0) {
	config->display_timeout_value = atoi(value);
      } else if (strcmp(name, "fish-update-interval") == 0) {
	config->fish_update_interval = atoi(value);
      }
    }
  }

  free(line);
  fclose(fp);

  return 1;
}


int parse_aquarium_file(const char *filename)
{
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) { 
    perror("fopen");
    return 0; 
  }

  char *line = NULL;
  size_t len = 0;
  if (getline(&line, &len, fp) == -1) {
    return 0;
  }

  int aquarium_width, aquarium_height;
  if (sscanf(line, "%dx%d", &aquarium_width, &aquarium_height) != 2) {
    perror("sscanf");
    return 0;
  }

  aquarium_init(aquarium_width, aquarium_height);

  while (getline(&line, &len, fp) != -1) {
    char name[MAX_PARAM_LENGTH];
    int x, y, width, height;
    if (sscanf(line, "%s %dx%d+%d+%d", name, &x, &y, &width, &height) != 5) {
      perror("sscanf");
      continue;
    }

    if (x < 0 || x + width > aquarium_width
	|| y < 0 || y + height > aquarium_height) {
      fprintf(stderr, "View %s not loaded because values are not valid.\n", name);
      continue;
    }

    view_add(name, x, y, width, height);
  }

  free(line);
  fclose(fp);

  return 1;
}
