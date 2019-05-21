#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "aquarium.h"
#include "tools.h"
#include "mobility.h"
#include "fish.h"


#define MOBILITY_RUNNING 0
#define MOBILITY_STOPPED 1


struct random_path_param {
  time_ms time_to_arrival;
  struct coordinates end_point;
};


void *chaotic(struct fish *fish);
void *random_path(struct fish* fish);
void *horizontal_path(struct fish* fish);

void random_param_init(struct fish *fish);
void horizontal_param_init(struct fish *fish);
void no_mobility_param_init(struct fish *fish);


static void *mobility_functions[NO_MOBILITY] = {random_path, horizontal_path, chaotic};

static void *mobility_params[NO_MOBILITY] = {random_param_init, horizontal_param_init, no_mobility_param_init};
static char *mobility_names[NO_MOBILITY] = {"RandomWayPoint", "HorizontalPathWay", "Chaotic"};

pthread_t update_thread;

static int mobility_status = MOBILITY_STOPPED;

struct size aquarium_size;


int mobility_from_name(char *name)
{
  for (enum mobility_function f = 0; f != NO_MOBILITY; f++) {
    if (strcmp(name, mobility_names[f]) == 0) {
      return f;
    }
  }
  return NO_MOBILITY;
}


void *updater(void *param)
{
  (void)param;
  //printf("Mobiliy started\n");
  while (mobility_status == MOBILITY_RUNNING) {
    fishs_update();
    msleep(DT);
  }
  pthread_exit(NULL);
}


void mobility_init(struct aquarium *aquarium)
{
  srand(time(NULL));
  aquarium_size = aquarium->size;
  mobility_status = MOBILITY_RUNNING;
  pthread_create(&update_thread, NULL, &updater, NULL);
}


void mobility_finalize()
{
  mobility_status = MOBILITY_STOPPED;
  pthread_join(update_thread, NULL);
}


time_ms get_time (void)
{
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  time_ms t = spec.tv_nsec / 1000000 + spec.tv_sec*1000;
  return t;
}


void random_point(struct coordinates *c)
{
  c->x = rand() % aquarium_size.width;
  c->y = rand() % aquarium_size.height;
}


void *no_mobility(struct fish* fish)
{
  (void)fish;
  return NULL;
}


void *chaotic(struct fish *fish)
{
  int shiftX[] = {-10, 0, 10};
  int shiftY[] = {-10, 0, 10};
  const struct size aquarium_size = aquarium_get_size();
    
  if (fish->coordinates.x <= 10) {
    shiftX[0] = 10;
  } else if (fish->coordinates.x >= aquarium_size.width - 10) {
    shiftX[2] = -10;
  }

  if (fish->coordinates.y <= 10) {
    shiftY[0] = 10;
  } else if (fish->coordinates.y >= aquarium_size.height - 10) {
    shiftY[2] = -10;
  }
  
  fish->coordinates.x += shiftX[rand() % 3];  
  fish->coordinates.y += shiftY[rand() % 3];

  return NULL;
}


void *random_path(struct fish* fish)
{
  struct random_path_param *p = fish->param;

  //printf("time to arrival : %ld, pos : %dx%d\n", p->time_to_arrival,
  //	 fish->coordinates.x, fish->coordinates.y);
  if (p->time_to_arrival <= DT) {
    p->time_to_arrival = RANDOM_TIME_MAX;
    random_point(&p->end_point);
    //printf("New dest : %dx%d\n", p->end_point.x, p->end_point.y);
    return NULL;
  }
  
  time_ms k =  p->time_to_arrival / DT;
  fish->coordinates.x += (p->end_point.x - fish->coordinates.x) / k;
  fish->coordinates.y += (p->end_point.y - fish->coordinates.y) / k;
  p->time_to_arrival -= DT;
  return NULL;
}


void *horizontal_path(struct fish* fish)
{
  struct random_path_param *p = fish->param;

  //printf("time to arrival : %ld, pos : %dx%d\n", p->time_to_arrival,
  //	 fish->coordinates.x, fish->coordinates.y);
  if (p->time_to_arrival <= DT) {
    p->time_to_arrival = RANDOM_TIME_MAX;
    const int aquarium_width = aquarium_get_size().width;
    p->end_point.x = fish->coordinates.x > aquarium_width / 2 ? 1
      : aquarium_width - fish->size.width - 1;
    
    //printf("New dest : %dx%d\n", p->end_point.x, p->end_point.y);
    return NULL;
  }
  
  time_ms k =  p->time_to_arrival / DT;
  fish->coordinates.x += (p->end_point.x - fish->coordinates.x) / k;
  p->time_to_arrival -= DT;
  return NULL;
}


void random_param_init(struct fish *fish)
{
  struct random_path_param *p = malloc(sizeof(struct random_path_param));
  p->time_to_arrival = 0;
  fish->param = p;
}


void horizontal_param_init(struct fish *fish)
{
  struct random_path_param *p = malloc(sizeof(struct random_path_param));
  p->time_to_arrival = DT;
  const int aquarium_width = aquarium_get_size().width;
  p->end_point.x = fish->coordinates.x > aquarium_width / 2 ? 1
    : aquarium_width - fish->size.width - 1;
  p->end_point.y = fish->coordinates.y;
  fish->param = p;
}


void no_mobility_param_init(struct fish *fish)
{ (void)fish; }


void call_mobility_function(struct fish *fish)
{
  //  printf("mobility call\n");
  if (fish->state == STARTED) {
    fish->mobility_function(fish);
  }
}


int setup_mobility(struct fish* fish, char *mobility)
{
  //  printf("Mobility : %s\n", mobility);
  strcpy(fish->mobility_name, mobility);
  int f = mobility_from_name(mobility);
  //printf("%d\n", f);
  
  if (f == NO_MOBILITY) {
    fish->mobility_function = &no_mobility;
    no_mobility_param_init(fish);
    return 0;
  }
  
  fish->mobility_function = mobility_functions[f];
  void *(*param_init)(struct fish*) = mobility_params[f];
  param_init(fish);
  return 1;
}
