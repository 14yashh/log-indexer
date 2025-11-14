#include "index.h"
#include "cli.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static void trim(char *str) {
    char *end;
    
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return;
    
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
}


time_t parseTimeString(const char *timeString) {
    struct tm tm_time = {0};
    
    if (sscanf(timeString, "%d-%d-%d-%d:%d:%d",
               &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday,
               &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec) != 6) {
        fprintf(stderr, "Error: Invalid time format. Please use: YYYY-MM-DD-HH:MM:SS\n");
        return (time_t)-1;
    }
    
    tm_time.tm_year -= 1900;
    tm_time.tm_mon -= 1;
    
    return mktime(&tm_time);
}


void interactiveQuery(const char *logFile, const char *indexFile) {
    char input[100];
    char timeStringBuffer[100];
    
    printf("\n======== Log Query Mode ========\n");
    printf("Available log levels: DEBUG, INFO, WARN, ERROR, CRITICAL\n");
    printf("Commands:\n");
    printf("  - Enter a log level to search (e.g., ERROR, INFO)\n");
    printf("  - Type 'stats' to see index statistics\n");
    printf("  - Type 'all' to see all log levels\n");
    printf("  - Type 'time' to search by time range\n");
    printf("  - Type 'quit' to exit\n");
    printf("=====================================\n\n");
    
    while (1) {
        printf("Enter log level or command: ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        trim(input);
        
        for (int i = 0; input[i]; i++) {
            input[i] = toupper(input[i]);
        }
        
        if (strlen(input) == 0) {
            continue;
        }
        
        if (strcmp(input, "QUIT") == 0 || strcmp(input, "EXIT") == 0) {
            printf("Exiting...\n");
            break;
        } 
        else if (strcmp(input, "STATS") == 0) {
            displayIndexStats(indexFile);
        } 
        else if (strcmp(input, "TIME") == 0) {
            time_t startTime, endTime;

            printf("  Enter start time (YYYY-MM-DD-HH:MM:SS): ");
            if (fgets(timeStringBuffer, sizeof(timeStringBuffer), stdin) == NULL) break;
            timeStringBuffer[strcspn(timeStringBuffer, "\n")] = 0;
            trim(timeStringBuffer);
            startTime = parseTimeString(timeStringBuffer);
            
            if (startTime == (time_t)-1) {
                fprintf(stderr, "Invalid start time.\n\n");
                continue;
            }

            printf("  Enter end time   (YYYY-MM-DD-HH:MM:SS): ");
            if (fgets(timeStringBuffer, sizeof(timeStringBuffer), stdin) == NULL) break;
            timeStringBuffer[strcspn(timeStringBuffer, "\n")] = 0;
            trim(timeStringBuffer);
            endTime = parseTimeString(timeStringBuffer);

            if (endTime == (time_t)-1) {
                fprintf(stderr, "Invalid end time.\n\n");
                continue;
            }

            if (endTime < startTime) {
                fprintf(stderr, "Error: End time must be after start time.\n\n");
                continue;
            }
            queryByTime(logFile, indexFile, startTime, endTime);
        }
        else if (strcmp(input, "ALL") == 0) {
            printf("\nShowing all log levels:\n");
            const char *levels[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
            for (int i = 0; i < 5; i++) {
                printf("\n--- %s logs ---\n", levels[i]);
                int count = queryByLevel(logFile, indexFile, levels[i]);
                if (count == 0) {
                    printf("(No %s logs found)\n", levels[i]);
                }
            }
        } 
        else {
            int count = queryByLevel(logFile, indexFile, input);
            if (count == 0) {
                printf("No logs found for level: %s\n", input);
            }
        }
        printf("\n");
    }
}