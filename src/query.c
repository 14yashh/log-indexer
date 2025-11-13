#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int queryByTime(const char *logFile, const char *indexFile, time_t startTime, time_t endTime) {
    int logFileDescriptor, indexFileDescriptor;
    indexEntry entry;
    char lineBuffer[MAX_LINE];
    int matchCount = 0;

    indexFileDescriptor = open(indexFile, O_RDONLY | O_BINARY);
    if (indexFileDescriptor == -1) {
        perror("failed to open index file!");
        return -1;
    }

    logFileDescriptor = open(logFile, O_RDONLY | O_BINARY);
    if (logFileDescriptor == -1) {
        perror("failed to open log file!");
        return -1;
    }

    printf("querying logs from %s to %s\n", ctime(&startTime), ctime(&endTime));
    printf("------------------------------------------------------------\n");
    fflush(stdout);

    while (read(indexFileDescriptor, &entry, sizeof(indexEntry)) == sizeof(indexEntry)) {
        if (entry.timeStamp >= startTime && entry.timeStamp <= endTime) {
            if (lseek(logFileDescriptor, entry.offset, SEEK_SET) == -1) {
                perror("lseek failed!");     // if cursor dont jump to offset
                continue;
            }

            memset(lineBuffer, 0, MAX_LINE);

            size_t readLen = entry.lineLength;
            if (readLen > MAX_LINE - 1) {
                readLen = MAX_LINE - 1;     // truncate line to safe size (1024)
            }

            ssize_t bytesRead = read(logFileDescriptor, lineBuffer, readLen);
            
            if (bytesRead > 0) {
                lineBuffer[bytesRead] = '\0';
                printf("%s", lineBuffer);
                fflush(stdout);
                matchCount++;
            }
        }
    }
    printf("------------------------------------------------------------");
    printf("found %d matching entries\n", matchCount);

    close(logFileDescriptor);
    close(indexFileDescriptor);
    return matchCount;
}

int queryByLevel(const char *logFile, const char *indexFile, const char *level) {
    int logFileDescriptor, indexFileDescriptor;
    indexEntry entry;
    char lineBuffer[MAX_LINE];
    int matchCount = 0;

    indexFileDescriptor = open(indexFile, O_RDONLY | O_BINARY);
    if (indexFileDescriptor == -1) {
        perror("failed to open index file!");
        return -1;
    }

    logFileDescriptor = open(logFile, O_RDONLY | O_BINARY);
    if (logFileDescriptor == -1) {
        perror("failed to open log file!");
        return -1;
    }

    printf("Querying logs with level %s\n", level);
    printf("------------------------------------------------------------\n");
    fflush(stdout);

    while (read(indexFileDescriptor, &entry, sizeof(indexEntry)) == sizeof(indexEntry)) {
        if (strcmp(entry.level, level) == 0) {
            if (lseek(logFileDescriptor, entry.offset, SEEK_SET) == -1) {
                perror("lseek failed!");     // if cursor dont jump to offset
                continue;
            }

            memset(lineBuffer, 0, MAX_LINE);

            size_t readLen = entry.lineLength;
            if (readLen > MAX_LINE - 1) {
                readLen = MAX_LINE - 1;     // truncate line to safe size (1024)
            }

            ssize_t bytesRead = read(logFileDescriptor, lineBuffer, readLen);
            
            if (bytesRead > 0) {
                lineBuffer[bytesRead] = '\0';
                printf("%s", lineBuffer);
                fflush(stdout);
                matchCount++;
            }
        }
    }
    printf("------------------------------------------------------------");
    printf("\nfound %d matching entries\n", matchCount);

    close(logFileDescriptor);
    close(indexFileDescriptor);
    return matchCount;
}

void displayIndexStats(const char *indexFile) {
    int indexFileDescriptor;
    indexEntry entry;
    off_t fileSize;
    int entryCount = 0;

    indexFileDescriptor = open(indexFile, O_RDONLY | O_BINARY);
    if (indexFileDescriptor == -1) {
        perror("failed to open index file!");
        return;
    }

    fileSize = lseek(indexFileDescriptor, 0, SEEK_END);
    if (fileSize == -1) {
        perror("failed to get file size!");
        close(indexFileDescriptor);
        return;
    }

    entryCount = fileSize/sizeof(indexEntry);

    lseek(indexFileDescriptor, 0, SEEK_SET);

    printf("\n==== Index Stats ====\n");
    printf("Total Entries: %d\n", entryCount);
    printf("Index File Size: %ld bytes\n", (long)fileSize);
    printf("Each Entry Size: %lu bytes\n", (unsigned long)sizeof(indexEntry));

    if (entryCount > 0) {
        if (read(indexFileDescriptor, &entry, sizeof(indexEntry)) == sizeof(indexEntry)) {
            printf("\nfirst log: %s", ctime(&entry.timeStamp));
        }
        if (lseek(indexFileDescriptor, -(off_t)sizeof(indexEntry), SEEK_END) != -1) {
            if (read(indexFileDescriptor, &entry, sizeof(indexEntry)) == sizeof(indexEntry)) {
                printf("last log: %s\n", ctime(&entry.timeStamp));
            }
        }
    }
    else {
        printf("empty index!\n");
    }
    close(indexFileDescriptor);
}
