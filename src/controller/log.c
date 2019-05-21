#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "log.h"


FILE *fp = NULL;
int level = 0;

void write_datetime()
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  const struct tm *tm = gmtime(&ts.tv_sec);

  fprintf(fp, "[%.2d/%.2d/%.4d %.2d:%.2d:%.2d:%3.ld] ", tm->tm_mday, tm->tm_mon, tm->tm_year,
	   tm->tm_hour, tm->tm_min, tm->tm_sec, ts.tv_nsec / 1000000);
}


void log_init(char *filename, const int verbosity_level)
{
  if ((fp = fopen(filename, "a")) == NULL) {
    perror("fopen");
    return;
  }

  level = verbosity_level;
  
  write_datetime();
  fprintf(fp, "Controller started\n");
}


void log_write(const int verbosity_level, char *format, ...)
{
  if (fp == NULL || verbosity_level > level) {
    return;
  }

  write_datetime();
  
  va_list valist;
  va_start(valist, format);
  vfprintf(fp, format, valist);
  va_end(valist);
  
  fprintf(fp, "\n");
}


void log_finalize()
{
  if (fp == NULL) {
    return;
  }
  
  write_datetime();
  fprintf(fp, "Controller finished\n");
  fclose(fp);
}



  
