#include "Library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int SLOTS;
library_t library;

/*
    Function declarations
*/
void setSlots(int slots);
void processEvent(char *data);
void insertGenre(library_t *library, genre_t *genreNode);
void insertBook(genre_t *genre, book_t *book);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("No Arguments, end program\n");
        return 1;
    }
    else
    {
        printf("LIBRARY MANAGEMENT SYSTEM PROJECT\ncsd5503\n--------------------------\n");
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
    genre_t *g = library.genres;
    while (g != NULL)
    {
        if (g->gid == 10)
        {
            printf("Head: %s\n",g->books->title);
            break;
        }
        g= g->next;
    }
    return 0;
}

/*
    Function to process events and it's data
    It reads the line using sscanf by describing the format of the line.
    Based on the line (data) it takes, it runs a different event.
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
                printf("IGNORED\n");
                return;
            }
            /* Initialize Genre Node*/

            genreNode->gid = genre_id;
            genreNode->books = NULL;
            genreNode->next = NULL;
            strcpy(genreNode->name, genre_name);

            /* Insert the genre in the singly linked list for genres */
            insertGenre(&library, genreNode);
        }
    }

    else if (strncmp(data, "BK ", 3) == 0)
    {
        int book_ID;
        int genre_ID;
        char book_title[TITLE_MAX];
        /* %[^\"] reads all chars until closing quotation character (Book title) "*/
        if (sscanf(data, "BK %d %d \"%[^\"]\"", &book_ID, &genre_ID, book_title) == 3)
        {
            book_t *bookNode = (book_t *)malloc(sizeof(book_t));
            if (bookNode == NULL)
            {
                printf("Failure to allocate book memory\n");
                printf("IGNORED\n");
                return;
            }
            bookNode->bid = book_ID;
            bookNode->gid = genre_ID;
            bookNode->prev = NULL;
            bookNode->lost_flag=0;
            bookNode->n_reviews=0;
            bookNode->avg = 0;
            bookNode->sum_scores= 0;
            strcpy(bookNode->title, book_title);
            printf("%d %d %s\n", bookNode->gid, bookNode->bid, bookNode->title);
            genre_t *tmp = library.genres;
            if (tmp == NULL)
            {
                printf("GENRE IN BOOK IGNORED\n");
                return;
            }
            /*
                Check if head of books list has the same gid as the book
                previously this error caused a segfault.
            */
            // if (tmp->gid == bookNode->gid)
            // {
            //     insertBook(tmp, bookNode);
            //     return;
            // }
            /* 
                Check if rest of doubly linked list has gid
            */
            // while (tmp != NULL && tmp->gid != bookNode->gid)
            // {
            //     printf("%d %d %s\n", bookNode->gid, bookNode->bid, bookNode->title);
            //     printf("%s %d\n", tmp->name, tmp->gid);
            //     if (tmp->gid == bookNode->gid)
            //     {
            //         insertBook(tmp, bookNode);
            //         printf("%d\n", tmp->gid);
            //         return;
            //     }
            //     else
            //     {
            //         printf(" BOOK IGNORED\n");
            //         return;
            //     }
            //     tmp = tmp->next;
            // }
        }
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
    Function to insert Genre based on its ID, this will keep the Genre List sorted.
    Function runs on G event.
*/
void insertGenre(library_t *library, genre_t *genreNode)
{
    genre_t *tmp = library->genres;
    genre_t *prev = NULL;
    /* If the genre list is empty, make the new Genre it's head. */
    if (library->genres == NULL)
    {
        library->genres = genreNode;
        printf("DONE\n");
        return;
    }
    /* If list is not NULL and genreNode gid < head gid, make genreNode head of the list */
    if (genreNode->gid < tmp->gid)
    {
        genreNode->next = tmp;
        library->genres = genreNode;
        printf("DONE\n");
        return;
    }

    /* Traverse until second to last node to check for duplicates*/
    while (tmp != NULL && tmp->gid < genreNode->gid)
    {
        prev = tmp;
        tmp = tmp->next;
    }

    /* Check last genre node to ensure no duplicates*/
    if (tmp != NULL && tmp->gid == genreNode->gid || (prev != NULL && prev->gid == genreNode->gid))
    {
        free(genreNode);
        printf("IGNORED\n");
        return;
    }

    /* If no duplicate gid is found, insert the genre to the singly linked list*/
    genreNode->next = tmp;
    if (prev != NULL)
    {
        prev->next = genreNode;
    }
    printf("DONE\n");
}

/*
    Function to insert Book based on its ID, this will keep the Book doubly linked List.
    The insertion is sorted based on the avg rating (highest to lowest).
    If rating is equal, sorting is based on bid
    Function runs on BK event.
*/
void insertBook(genre_t *genre, book_t *book)
{
    book_t *tmp;
    if (tmp == NULL)
    {
        genre->books = book;
        book->next = NULL;
        printf("DONE\n");
        return;
    }
}
