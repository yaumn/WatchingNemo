#ifndef VIEW_H
#define VIEW_H
#include "tools.h"
#include "queue.h"
#define NUMBER_MAX_VIEW 64



struct view {
  int id;
  char name[256];
  struct coordinates start;
  struct size size;
  int is_available;

  TAILQ_ENTRY(view) queue_entries;
};


void views_init();
void views_print();
void views_save(FILE *f);
void views_finalize();

void view_add(char *name, int x, int y, int width, int height);
void view_print(struct view *view);
void view_save(FILE *f, struct view *view);
void view_remove(struct view *view);
void view_remove_by_name(const char *name);
int view_get_number();
struct view *view_find(char *name);

void print_view_added();
void print_view_deleted(int id);

int view_set_available(char *name, const int available);
char *view_find_available();


#endif
