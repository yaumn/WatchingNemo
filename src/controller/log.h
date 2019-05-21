#ifndef LOG_H
#define LOG_H


void log_init(char *filename, const int verbosity_level);
void log_write(const int verbosity_level, char *format, ...);
void log_finalize();


#endif // LOG_H
