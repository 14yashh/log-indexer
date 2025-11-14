#ifndef LOG_CLI_H
#define LOG_CLI_H

#include <time.h>

time_t parseTimeString(const char *timeString);
void interactiveQuery(const char *logFile, const char *indexFile);

#endif