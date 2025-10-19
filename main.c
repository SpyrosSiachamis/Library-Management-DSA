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
        if (file == NULL)
        {
            printf("Error opening file, end program\n");
            return 1;
        }
        while ((fgets(line, sizeof(line), file)))
        {
            /* Ignores newline character and comments */
            if (line[0] == '\n' || line[0] == '#')
            {
                continue;
            }
            processEvent(line);
        }
        fclose(file);
    }
    /* Genre printing test */
    genre_t *tmp = library.genres;
    while (tmp != NULL)
    {
        printf("Genre Name: %s | Genre ID: %d\n", tmp->name, tmp->gid);
        tmp = tmp->next;
    }
    return 0;
}

/*
    Sets the total number of available display slots for the library.
    These get allocated later on for every genre respectively.
*/
void setSlots(int slots)
{
    SLOTS = slots;
    printf("DONE\n");
}

/*
    Function to process events and it's data
    It reads the line using sscanf by describing the format of the line.
*/
void processEvent(char *data)
{

    if (strncmp(data, "S", 1) == 0)
    {
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
    else if (strncmp(data, "G", 1) == 0)
    {
        int genre_id;
        char genre_name[NAME_MAX];

        /* %[^\"] reads all chars until closing quotation character "*/
        if (sscanf(data, "G %d \"%[^\"]\"", &genre_id, genre_name) == 2)
        {
            genre_t *genreNode = (genre_t *)malloc(sizeof(genre_t));
            if (genreNode == NULL)
            {
                printf("Failure to allocate genre memory\n");
                printf("Ignored\n");
            }
            else
            {
                /* Initialize Genre Node*/

                genreNode->gid = genre_id;
                genreNode->books = NULL;
                genreNode->next = NULL;
                strcpy(genreNode->name, genre_name);

                /* If library.genres list is NULL, make the new node the head */
                if (library.genres == NULL)
                {
                    library.genres = genreNode;
                }
                else
                {
                    /*
                        ------------------------------------- SOS ------------------------------------------
                        Temporary insertion, planning to implement sorted insertion or to sort it afterwards
                    */
                    /* Traverse until second to last node to check for duplicates*/
                    genre_t *tmp = library.genres;
                    while (tmp->next != NULL)
                    {
                        if (tmp->gid == genreNode->gid)
                        {
                            free(genreNode);
                            printf("Ignored\n");
                            break;
                        }
                        tmp = tmp->next;
                    }
                    /* Check last genre node to ensure no duplicates*/
                    if (tmp->gid == genreNode->gid)
                    {
                        free(genreNode);
                        printf("Ignored\n");
                    }
                    /* If no duplicate gid is found, insert the genre to the singly linked list*/
                    else
                    {
                        tmp->next = genreNode;
                    }
                }
            }

            // if (tmp == NULL)
            // {
            //     library.genres
            // }
        }
        printf("DONE\n");
        // genre_t genre
    }

    else if (strncmp(data, "BK ", 3) == 0)
    {
        // Handle BK command
    }
    else if (strncmp(data, "M ", 2) == 0)
    {
        // Handle M command
    }
    else if (strncmp(data, "L ", 2) == 0)
    {
        // Handle L command
    }
    else if (strncmp(data, "R ", 2) == 0)
    {
        // Handle R command
    }
    else if (strncmp(data, "PG ", 3) == 0)
    {
        // Handle PG command
    }
    else if (strncmp(data, "D ", 2) == 0)
    {
        // Handle D command
    }
    else if (strncmp(data, "PD ", 3) == 0)
    {
        // Handle PD command
    }
    else if (strncmp(data, "PM ", 3) == 0)
    {
        // Handle PM command
    }
}