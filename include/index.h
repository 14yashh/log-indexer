#ifndef LOG_INDEX_H
#define LOG_INDEX_H

#include <time.h>
#include <sys/types.h>

#define MAX_LINE 1024
#define MAX_LEVEL 10

typedef struct {
    off_t offset;
    time_t timeStamp;
    char level[MAX_LEVEL];  // level of log message
    size_t lineLength;      // length of log line
} indexEntry;


// function to build index
int parseLogLine(const char *line, indexEntry *entry, off_t offset);
int buildIndex(const char *logFile, const char *indexFile);

// function to query 
void displayIndexStats(const char *indexFile);
int queryByLevel(const char *logFile, const char *indexFile, const char *level);
int queryByTime(const char *logFile, const char *indexFile, time_t startTime, time_t endTime);

#endif