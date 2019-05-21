#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "queue.h" 
#include "tools.h"
#include "view.h"


TAILQ_HEAD(view_queue, view);

struct view_queue* views = NULL;
int nb_view = 0;

static pthread_mutex_t mutex_views;

//static pthread_mutexattr_t recursive;


void views_init()
{
  views = malloc(sizeof(struct view_queue));
  TAILQ_INIT(views);
}


void view_add(char *name, int x, int y, int width, int height)
{
  struct view *view = malloc(sizeof(struct view));
  view->size.width = width;
  view->size.height = height;
  view->start.x = x;
  view->start.y = y;
  view->id = nb_view++;
  view->is_available = 1;
  strcpy(view->name, name);

  TAILQ_INSERT_TAIL(views, view, queue_entries);
}


void view_remove(struct view *view)
{
  TAILQ_REMOVE(views, view, queue_entries);
  nb_view--;
  free(view);
}


void view_remove_by_name(const char *name)
{
  struct view *current = NULL;
  TAILQ_FOREACH(current, views, queue_entries) {
    if (strcmp(current->name, name) == 0) {
      view_remove(current);
      return;
    }
  }
}


void view_print(struct view *view) {
  if (view != NULL) {
    printf("%s %dx%d+%d+%d\n", view->name, view->start.x, view->start.y, view->size.width, view->size.height);
  }
}


void view_save(FILE *f, struct view *view)
{
  if (view != NULL) {
    fprintf(f, "%s %dx%d+%d+%d\n", view->name, view->start.x, view->start.y, view->size.width, view->size.height);
  }
}


void views_print()
{
  if (!TAILQ_EMPTY(views)) {
    for (struct view* v = TAILQ_FIRST(views); v != TAILQ_LAST(views, view_queue); v = TAILQ_NEXT(v, queue_entries)) {
      view_print(v);
    }
    view_print(TAILQ_LAST(views, view_queue));
  }
}


void views_save(FILE *f)
{
  if (!TAILQ_EMPTY(views)) {
    for (struct view* v = TAILQ_FIRST(views); v != TAILQ_LAST(views, view_queue); v = TAILQ_NEXT(v, queue_entries)) {
      view_save(f, v);
    }
    view_save(f, TAILQ_LAST(views, view_queue));
  }
}


void views_finalize()
{
  if (views == NULL) {
    return;
  }
  
  while (!TAILQ_EMPTY(views)) {
    struct view * view = TAILQ_FIRST(views);
    view_remove(view);
  }
  free(views);
}


int view_get_number()
{
  return nb_view;
}


void print_view_added()
{
  printf("\t-> view added\n");
}

void print_view_deleted(int id)
{
  printf("\t-> view N%d deleted\n", id);
}


int view_set_available(char *name, const int available)
{
  pthread_mutex_lock(&mutex_views);
  
  struct view *view = NULL;
  TAILQ_FOREACH(view, views, queue_entries) {
    if (strcmp(view->name, name) == 0) {
      if (view->is_available != available) {
	view->is_available = available;
	pthread_mutex_unlock(&mutex_views);
	return 1;
      }
      break;
    }
  }

  pthread_mutex_unlock(&mutex_views);
  return 0;
}


char *view_find_available()
{
  pthread_mutex_lock(&mutex_views);
  struct view *view = NULL;
  TAILQ_FOREACH(view, views, queue_entries) {
    if (view->is_available) {
      view->is_available = 0;
      pthread_mutex_unlock(&mutex_views);
      return view->name;
    }
  }
  pthread_mutex_unlock(&mutex_views);

  return NULL;  
}


struct view *view_find(char *name) {
  pthread_mutex_lock(&mutex_views);

  struct view *view = NULL;
  TAILQ_FOREACH(view, views, queue_entries) {
    if (strcmp(view->name, name) == 0) {
      break;
    }
  }
  
  pthread_mutex_unlock(&mutex_views);
  return view;
}
