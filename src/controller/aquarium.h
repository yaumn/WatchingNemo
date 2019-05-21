#ifndef AQUARIUM_H
#define AQUARIUM_H


#include "tools.h"


struct aquarium {
  struct size size;
};

void aquarium_init(int width, int height);
int aquarium_load(char *aquarium_name);
int aquarium_save(char *aquarium_name);
void aquarium_show();
void aquarium_finalize();
void aquarium_print();
struct size aquarium_get_size();
int aquarium_is_loaded();

#endif
