#include "Library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int SLOTS;
library_t library;

void setSlots(int slots);
void processEvent(char *data);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("No Arguments, end program\n");
        return 1;
    }
    else
    {
        FILE *file = fopen(argv[1], "r");
        char line[256];
        printf("%s\n%s\n", argv[0], argv[1]);
        if (file == NULL)
        {
            printf("Error opening file, end program\n");
            return 1;
        }
        while ((fgets(line, sizeof(line), file)))
        {
            if (line[0] == '\n' || line[0] == '#')
            {
                continue;
            }
            processEvent(line);
        }
        fclose(file);
    }
    printf("%d", SLOTS);
    printf("\n");
    return 0;
}

void setSlots(int slots)
{
    SLOTS = slots;
    printf("DONE\n");
}
/*
    Helper function to process events and it's data
    It reads the line using sscanf by describing the format of the line.
*/
void processEvent(char *data)
{
    if (strncmp(data, "S ", 2) == 0) {
        int slots;
        if (sscanf(data, "S %d", &slots) == 1)
        {
            setSlots(slots);
        }
        else
        {
            printf("Failure to add slots.\n");
        }
    }
    else if (strncmp(data, "G ", 2) == 0)
    {
        /* code */
    }
    
    else if (strncmp(data, "BK ", 3) == 0) {
        // Handle BK command
    } else if (strncmp(data, "PG ", 3) == 0) {
        // Handle PG command
    }
        
}