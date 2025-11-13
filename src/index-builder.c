#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int parseLogLine(const char *line, indexEntry *entry, off_t offset) {
    struct tm tm_time = {0};
    char levelBuff[MAX_LEVEL];   // buffer to store log line

    entry->offset = offset;

    // format: YYYY-MM-DD HH:MM:SS
    if (sscanf(line, "%d-%d-%d %d:%d:%d [%9[^]]]",
               &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday,
               &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec, levelBuff) != 7) {
        return 0; // failure 
    }

    // adjust year and month
    tm_time.tm_year -= 1900;    
    tm_time.tm_mon -= 1;

    entry->timeStamp = mktime(&tm_time);
    strncpy(entry->level, levelBuff, MAX_LEVEL - 1);
    entry->level[MAX_LEVEL - 1] = '\0';

    return 1;
}

int buildIndex(const char *logFile, const char *indexFile) {
    FILE *logFilePointer;
    int indexFileDescriptor;
    char lineBuffer[MAX_LINE];
    off_t lineStart = 0;
    indexEntry entry;
    int entriesWritten = 0;

    logFilePointer = fopen(logFile, "rb");   // read binary mode
    if (logFilePointer == NULL){
        perror("failed to open log file!");
        return -1;
    }

    indexFileDescriptor = open(indexFile, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0644);
    if (indexFileDescriptor == -1) {
        perror("failed to create index file!");
        return -1;
    }

    printf("building index\n");

    lineStart = ftell(logFilePointer);

    while (fgets(lineBuffer, MAX_LINE, logFilePointer)) {
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0';

        if (parseLogLine(lineBuffer, &entry, lineStart)) {
            off_t nextLineStart = ftell(logFilePointer);

            entry.lineLength = nextLineStart - lineStart;

            if (write(indexFileDescriptor, &entry, sizeof(indexEntry)) == sizeof(indexEntry)) {
                entriesWritten++;
            }
        }

        lineStart = ftell(logFilePointer);
    }

    fclose(logFilePointer);
    close(indexFileDescriptor);

    printf("index built with %d entries\n", entriesWritten);
    return entriesWritten;
}
