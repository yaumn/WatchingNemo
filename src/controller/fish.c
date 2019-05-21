#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "fish.h"
#include "view.h"
#include "queue.h"
#include "mobility.h"
#include "tools.h"
#include "string.h"


TAILQ_HEAD(fish_queue, fish);

struct fish_queue* fishs;
static pthread_mutex_t mutex_fishs;

static pthread_mutexattr_t recursive;


int fish_belongs_to_view(struct fish *fish, struct view *view)
{
  return (fish->coordinates.x >= view->start.x && fish->coordinates.y >= view->start.y
	  && fish->coordinates.x <= view->start.x + view->size.width
	  && fish->coordinates.y <= view->start.y + view->size.height)
    || (fish->coordinates.x + fish->size.width >= view->start.x
	&& fish->coordinates.x + fish->size.width <= view->start.x + view->size.width
	&& fish->coordinates.y >= view->start.y && fish->coordinates.y <= view->start.y + view->size.height)
    || (fish->coordinates.x >= view->start.x && fish->coordinates.x <= view->start.x + view->size.width
	&& fish->coordinates.y + fish->size.height <= view->start.y + view->size.height
	&& fish->coordinates.y + fish->size.height >= view->start.y)
    || (fish->coordinates.x + fish->size.width <= view->start.x + view->size.width
	&& fish->coordinates.x + fish->size.width >= view->start.x
	&& fish->coordinates.y + fish->size.height <= view->start.y + view->size.height
	&& fish->coordinates.y + fish->size.height >= view->start.y);
}


void fishs_init()
{
  printf("Fishs init\n");
  fishs = malloc(sizeof(struct fish_queue));
  TAILQ_INIT(fishs);
  pthread_mutexattr_init(&recursive);
  pthread_mutexattr_settype(&recursive, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mutex_fishs, &recursive);
}


void fishs_finalize()
{
  if (fishs == NULL) {
    return;
  }

  while (!TAILQ_EMPTY(fishs)) {
    struct fish * fish = TAILQ_FIRST(fishs);
    fish_remove(fish->name);
  }
  
  free(fishs);
  fishs = NULL;
  pthread_mutex_destroy(&mutex_fishs);
  pthread_mutexattr_destroy(&recursive);
}


void fishs_update()
{
  pthread_mutex_lock(&mutex_fishs);
    if (!TAILQ_EMPTY(fishs)) {
      struct fish* f;
      for (f = TAILQ_FIRST(fishs); f != TAILQ_LAST(fishs, fish_queue); f = TAILQ_NEXT(f, queue_entries)) {
	fish_update(f);
      }
      fish_update(f);
    }
  pthread_mutex_unlock(&mutex_fishs);
}


void fish_update(struct fish *fish)
{
  call_mobility_function(fish);
}


int fish_add(char* name, int x, int y, int w, int h, char *mobility)
{
  if (fish_find(name) != NULL) {
    return 0;
  }

  struct fish *fish = malloc(sizeof(struct fish));
  fish->coordinates.x = x;
  fish->coordinates.y = y;
  fish->size.width = w;
  fish->size.height = h;
  fish->param = NULL;
  strcpy(fish->name, name);

  if (!setup_mobility(fish, mobility)) {
    return 0;
  }

  fish->state = NOT_STARTED;

  pthread_mutex_lock(&mutex_fishs);
  TAILQ_INSERT_TAIL(fishs, fish, queue_entries);
  pthread_mutex_unlock(&mutex_fishs);
  return 1;
}


int fish_remove(char *name)
{
  struct fish *fish;
  if ((fish = fish_find(name)) == NULL) {
    return 0;
  }

  pthread_mutex_lock(&mutex_fishs);
  TAILQ_REMOVE(fishs, fish, queue_entries);
  pthread_mutex_unlock(&mutex_fishs);
  free(fish->param);
  free(fish);
  return 1;
}


int fish_start(char *name)
{
  pthread_mutex_lock(&mutex_fishs);
  struct fish *f = fish_find(name);
  if (f == NULL || f->state == STARTED) {
    pthread_mutex_unlock(&mutex_fishs);
    return 0;
  }
  f->state = STARTED;
  pthread_mutex_unlock(&mutex_fishs);
  return 1;
}


struct fish *fish_find(char *name)
{
  pthread_mutex_lock(&mutex_fishs);
  struct fish *f;
  if (!TAILQ_EMPTY(fishs)) {
    for (f = TAILQ_FIRST(fishs); f != TAILQ_LAST(fishs, fish_queue); f = TAILQ_NEXT(f, queue_entries)) {
      if (strcmp(f->name,name) == 0) {
	pthread_mutex_unlock(&mutex_fishs);
	return f;
      }
    }
    if (strcmp(f->name,name) == 0) {
      pthread_mutex_unlock(&mutex_fishs);
      return f;
    }
  }
  pthread_mutex_unlock(&mutex_fishs);
  return NULL;
}


char *fish_get_info(struct view *view, struct fish *fish)
{
  const int length = MAX_BUFFER_SIZE + 50;
  char *info = malloc(length * sizeof(char));
  
  snprintf(info, length - 1, "[%s at %dx%d, %dx%d, 0]", fish->name,
	   (fish->coordinates.x - view->start.x) * 100 / view->size.width,
	   (fish->coordinates.y - view->start.y) * 100 / view->size.height,
	   fish->size.width * 100 / view->size.width,
	   fish->size.height * 100 / view->size.height);
  return info;
}


char *fishes_get_info(struct view *client_view)
{
  pthread_mutex_lock(&mutex_fishs);
  
  struct fish *fish = NULL;
  int total_length = 10;
  TAILQ_FOREACH(fish, fishs, queue_entries) {
    if (fish_belongs_to_view(fish, client_view)) {
      total_length += MAX_BUFFER_SIZE + 50;
    }
  }

  char *info_list = malloc(total_length * sizeof(char));
  strcpy(info_list, "list");
  
  fish = NULL;
  TAILQ_FOREACH(fish, fishs, queue_entries) {
    if (fish_belongs_to_view(fish, client_view)) {
      char *fish_info = fish_get_info(client_view, fish);
      strcat(info_list, " ");
      strcat(info_list, fish_info);
      free(fish_info);
    }
  }
  pthread_mutex_unlock(&mutex_fishs);

  strcat(info_list, "\n");
  return info_list;
}
