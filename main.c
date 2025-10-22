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
void insertMember(library_t *library, member_t *member);
void printGenre(library_t *library, int gid);
void insertLoan(member_t *member, loan_t *loan);
book_t *createBook(int gid, int bid, char title[NAME_MAX]);
genre_t *createGenre(int gid, char name[NAME_MAX]);
member_t *createMember(int sid, char name[NAME_MAX]);
loan_t *createLoan(int sid, int bid);
loan_t *createSentinelNode(int sid);

/*
    ----------------------- SOS -----------------------------
    Add an int return to the process event function to assure successfull event,
    if everything in the interior functions were successful, something could go 
    wrong in the handler after everything is successful. So before I print done I
    should check if the process event handler worked correctly. 
*/


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
    /* Print all members and their loans */
    printf("\n=== MEMBERS AND LOANS ===\n");
    member_t *m = library.members;

    if (m == NULL)
    {
        printf("No members in library.\n");
    }
    else
    {
        while (m != NULL)
        {
            printf("Member %d: %s\n", m->sid, m->name);
            
            /* Print loans for this member */
            loan_t *l = m->loans;
            if (l == NULL)
            {
                printf("  No loans\n");
            }
            else
            {
                printf("  Loans: ");
                while (l != NULL && l->bid != -1)  // Stop at sentinel node
                {
                    printf("%d ", l->bid);
                    l = l->next;
                }
                printf("\n");
            }
            
            m = m->next;
        }
    }
    printf("=== END MEMBERS AND LOANS ===\n\n");
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
            /* Initialize Genre Node*/
            genre_t *genre = createGenre(genre_id, genre_name);
            if (genre == NULL)
            {
                printf("G IGNORED\n");
                return;
            }

            /* Insert the genre in the singly linked list for genres */
            insertGenre(&library, genre);
            return;
        }
        else
        {
            printf("Failure to read G event\n");
            return;
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
            book_t *bookNode = createBook(genre_ID, book_ID, book_title);
            if (bookNode == NULL)
            {
                printf("BK IGNORED\n");
                return;
            }

            genre_t *genre = library.genres;
            int found = 0;

            while (genre != NULL)
            {
                if (genre->gid == bookNode->gid)
                {
                    found = 1;
                    insertBook(genre, bookNode);
                    break;
                }
                genre = genre->next;
            }

            if (!found)
            {
                free(bookNode);
                printf("BK IGNORED\n");
            }
        }
        else
        {
            printf("Failure to read BK event\n");
        }
    }
    else if (strncmp(data, "M ", 2) == 0)
    {
        int sid;
        char name[NAME_MAX];
        /* %[^\"] reads all chars until closing quotation character (Book title) "*/
        if (sscanf(data, "M %d \"%[^\"]\"", &sid, name) == 2)
        {
            member_t *memberNode = createMember(sid, name);
            if (memberNode == NULL)
            {
                printf("M IGNORED\n");
                return;
            }
            insertMember(&library, memberNode);
        }
        else
        {
            printf("Failure to read M event\n");
        }
    }
    else if (strncmp(data, "L ", 2) == 0)
    {
        int sid;
        int bid;
        if (sscanf(data, "L %d %d", &sid, &bid) == 2)
        {
            loan_t *loan = createLoan(sid,bid);
            if (loan == NULL)
            {
                printf("L IGNORED\n");
                return;
            }
            member_t *current = library.members;
            while (current != NULL && current->sid != loan->sid)
            {
                current = current->next;
            }
            if (current->sid == loan->sid && current != NULL)
            {
                insertLoan(current,loan);
                return;
            }
            printf("L IGNORED\n");
            free(loan);
        }
        else
        {
            printf("L IGNORED\n");
        }
    }
    else if (strncmp(data, "R ", 2) == 0)
    {
        // Handle R command
    }
    else if (strncmp(data, "PG ", 3) == 0)
    {
        int gid;
        if (sscanf(data, "PG %d", &gid) == 1)
        {
            printGenre(&library, gid);
            return;
        }
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
    printf("S DONE\n");
}
/* Helper Function to create Genre Node */
genre_t *createGenre(int gid, char name[NAME_MAX])
{
    genre_t *genre = (genre_t *)malloc(sizeof(genre_t));
    if (genre == NULL)
    {
        printf("Failure to allocate genre memory\n");
        printf("G IGNORED\n");
        return NULL;
    }

    genre->gid = gid;
    genre->books = NULL;
    genre->next = NULL;
    strcpy(genre->name, name);
    return genre;
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
        printf("G DONE\n");
        return;
    }
    /* If list is not NULL and genreNode gid < head gid, make genreNode head of the list */
    if (genreNode->gid < tmp->gid)
    {
        genreNode->next = tmp;
        library->genres = genreNode;
        printf("G DONE\n");
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
        printf("G IGNORED\n");
        return;
    }

    /* If no duplicate gid is found, insert the genre to the singly linked list*/
    genreNode->next = tmp;
    if (prev != NULL)
    {
        prev->next = genreNode;
    }
    printf("G DONE\n");
}

/* Helper Function to create Book Node */
book_t *createBook(int gid, int bid, char title[NAME_MAX])
{
    book_t *book = (book_t *)malloc(sizeof(book_t));
    if (book == NULL)
    {
        printf("Failure to allocate book memory\n");
        printf("BK IGNORED\n");
        return NULL;
    }

    // Initialize book
    book->bid = bid;
    book->gid = gid;
    book->next = NULL;
    book->prev = NULL;
    book->lost_flag = 0;
    book->n_reviews = 0;
    book->avg = 0;
    book->sum_scores = 0;
    strcpy(book->title, title);
    return book;
}

/* Helper Function to create Member Node */
member_t *createMember(int sid, char name[NAME_MAX])
{
    member_t *memberNode = (member_t *)malloc(sizeof(member_t));
    if (memberNode == NULL)
    {
        printf("M IGNORED\n");
        return NULL;
    }
    memberNode->loans = NULL;
    memberNode->sid = sid;
    strcpy(memberNode->name, name);
    return memberNode;
}

loan_t *createLoan(int sid, int bid)
{
    loan_t *loan = (loan_t*)malloc(sizeof(loan_t));
    if (loan == NULL)
    {
        printf("L IGNORED\n");
        return NULL;
    }
    loan->sid = sid;
    loan->bid = bid;
    return loan;
}

loan_t *createSentinelNode(int sid)
{
    loan_t *sentinel = (loan_t*)malloc(sizeof(loan_t));
    if (sentinel == NULL)
    {
        printf("L IGNORED\n");
        return NULL;
    }
    sentinel->next = NULL;
    sentinel->bid = -1;
    sentinel->sid = sid;
    return sentinel;
}

/*
    Function to insert Book based on its ID, this will keep the Book doubly linked List.
    The insertion is sorted based on the avg rating (highest to lowest).
    If rating is equal, sorting is based on bid
    Function runs on BK event.
*/
void insertBook(genre_t *genre, book_t *bookNode)
{
    /* Check for duplicate bid */
    book_t *tmp = genre->books;
    while (tmp != NULL)
    {
        if (tmp->bid == bookNode->bid)
        {
            free(bookNode);
            printf("BK IGNORED\n");
            return;
        }
        tmp = tmp->next;
    }

    /* Insert at head if books == NULL */
    if (genre->books == NULL)
    {
        genre->books = bookNode;
        bookNode->next = NULL;
        bookNode->prev = NULL;
        printf("BK DONE\n");
        return;
    }

    /* Insert in sorted position */
    book_t *current = genre->books;
    book_t *prev = NULL;

    /* Find insertion point */
    while (current != NULL)
    {
        /* Higher avg */
        if (bookNode->avg > current->avg)
        {
            break;
        }
        /* Same avg, lower bid first */
        if (bookNode->avg == current->avg && bookNode->bid < current->bid)
        {
            break;
        }
        prev = current;
        current = current->next;
    }

    /* Insert at head */
    if (prev == NULL)
    {
        bookNode->next = genre->books;
        bookNode->prev = NULL;
        genre->books->prev = bookNode;
        genre->books = bookNode;
    }
    /* Insert in middle or at end */
    else
    {
        bookNode->next = current;
        bookNode->prev = prev;
        prev->next = bookNode;
        if (current != NULL)
        {
            current->prev = bookNode;
        }
    }

    printf("BK DONE\n");
}

/*
    Function to insert Member based on its ID into the member list.
    The insertion maintains the member list structure.
    The insertion is sorted based on mid (lowest to highest).
    Function runs on M event.
*/
void insertMember(library_t *library, member_t *member)
{
    member_t *current = library->members;
    member_t *prev = NULL;
    /* If the member list is empty, make the new Member it's head. */
    if (library->members == NULL)
    {
        library->members = member;
        printf("M DONE\n");
        return;
    }
    /* If list is not NULL and member sid < head sid, make genreNode head of the list */
    if (member->sid < current->sid)
    {
        member->next = current;
        library->members = member;
        printf("M DONE\n");
        return;
    }

    /* Traverse until second to last node to check for duplicates*/
    while (current != NULL && current->sid < member->sid)
    {
        prev = current;
        current = current->next;
    }

    /* Check last member node to ensure no duplicates*/
    if (current != NULL && current->sid == member->sid || (prev != NULL && prev->sid == member->sid))
    {
        free(member);
        printf("M IGNORED\n");
        return;
    }

    /* If no duplicate gid is found, insert the member to the singly linked list*/
    member->next = current;
    if (prev != NULL)
    {
        prev->next = member;
    }
    printf("M DONE\n");
}


/*
    REMINDER: CHECK IF BOOK EXISTS IN ANY GENRE
*/
void insertLoan(member_t *member, loan_t *loan)
{
    /* If loans list is empty, create sentinel node for tail of loan then put it as the next pointer of the head loan node that is being added and then make the new loan as the HEAD of the list. */
    if (member->loans == NULL)
    {
        loan_t *sentinel = createSentinelNode(member->sid);
        if (sentinel == NULL)
        {
            printf("L IGNORED\n");
            free(loan);
            return;
        }
        loan->next = sentinel;
        member->loans = loan;
        printf("L DONE\n");
        return;
    }
    loan_t *current = member->loans;

    /* Traverse list until sentinel node OR until duplicate loan */
    while (current->next->bid != -1 && current->bid != loan->bid)
    {
        current = current->next;
    }
    if (current->bid == loan->bid)
    {
        printf("L IGNORED\n");
        free(loan);
        return;
    }
    /* If duplicate loan hasnt been found, create new loan */
    loan->next = current->next;
    current->next = loan;
    printf("L DONE\n");
}

/*
    Function to check if book list is sorted in a genre.
    prints every book's bid next to it's avg review score.
    Function runs on PG event.
*/
void printGenre(library_t *library, int gid)
{
    genre_t *tmp = library->genres;
    book_t *bTmp;
    if (tmp == NULL)
    {
        printf("PG IGNORED\n");
        return;
    }

    if (tmp->gid == gid)
    {
        bTmp = tmp->books;
        if (bTmp == NULL)
        {
            printf("PG IGNORED\n");
            return;
        }
        while (bTmp != NULL)
        {
            printf("%d %d\n", bTmp->bid, bTmp->avg);
            bTmp = bTmp->next;
        }
        return;
    }
    while (tmp != NULL && tmp->gid != gid)
    {
        tmp = tmp->next;
    }
    if (tmp != NULL)
    {
        bTmp = tmp->books;
        while (bTmp != NULL)
        {
            printf("%d %d\n", bTmp->bid, bTmp->avg);
            bTmp = bTmp->next;
        }
        return;
    }
    printf("PG IGNORED\n");
    return;
}
