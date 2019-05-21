#ifndef UTILS_H
#define UTILS_H


#include <pthread.h>

#ifdef WIN32
#include <windows.h>
#define msleep(ms) Sleep (ms)
#else
#define msleep(ms) usleep ((ms)*1000)
#endif


extern int terminate_program;
extern int sock;


struct controller_config
{
  int port;
  int display_timeout_value;
  int fish_update_interval;
};

struct coordinates
{
  int x;
  int y;
};

struct size
{
  int width;
  int height;
};

#endif
