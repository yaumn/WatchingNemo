#ifndef CLIENT_H
#define CLIENT_H

struct view;
struct fish;

struct client {
  int id;
  struct view *view;
  struct fish* *fishes;
};

#endif
