#include "index.h"
#include "cli.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s build <logfile> <indexfile>\n", argv[0]);
        printf("  %s query <logfile> <indexfile>          - Interactive mode\n", argv[0]);
        printf("  %s query-level <logfile> <indexfile> <level>\n", argv[0]);
        printf("  %s query-time <logfile> <indexfile> <start_time> <end_time>\n", argv[0]);
        printf("  %s stats <indexfile>\n\n", argv[0]);
        printf("Time format for query-time: \"YYYY-MM-DD-HH:MM:SS\"\n");
        return 1;
    }
    
    if (strcmp(argv[1], "build") == 0 && argc == 4) {
        buildIndex(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "query") == 0 && argc == 4) {
        interactiveQuery(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "query-level") == 0 && argc == 5) {
        queryByLevel(argv[2], argv[3], argv[4]);
    }
    else if (strcmp(argv[1], "query-time") == 0 && argc == 6) {
    
        time_t startTime = parseTimeString(argv[4]);
        time_t endTime = parseTimeString(argv[5]);

        if (startTime == (time_t) - 1 || endTime == (time_t) - 1) {
            fprintf(stderr, "Exiting due to invalid time format.\n");
            return 1;
        }
        
        if (endTime < startTime) {
            fprintf(stderr, "Error: End time must be after start time.\n");
            return 1;
        }
        queryByTime(argv[2], argv[3], startTime, endTime);
    }
    else if (strcmp(argv[1], "stats") == 0 && argc == 3) {
        displayIndexStats(argv[2]);
    }
    else {
        printf("Invalid command\n");
        return 1;
    }
    
    return 0;
}