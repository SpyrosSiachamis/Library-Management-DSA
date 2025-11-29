#include "Library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int SLOTS;
library_t library;


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
    return 0;
}

/*
    Function to process events and it's data
    It reads the line using sscanf by describing the format of the line.
    Based on the line (data) it takes, it runs a different event.
*/
void processEvent(char *data)
{
    int result = 1;
    if (strncmp(data, "S", 1) == 0)
    {
        int slots;
        if (sscanf(data, "S %d", &slots) == 1)
        {

            result = setSlots(slots);
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
                result = 1;
            }
            else
            {
                /* Insert the genre in the singly linked list for genres */
                result = insertGenre(&library, genre);
            }
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
            if (bookNode != NULL)
            {
                genre_t *genre = library.genres;
                int found = 0;

                while (genre != NULL)
                {
                    if (genre->gid == bookNode->gid)
                    {
                        found = 1;
                        result = insertBook(genre, bookNode);
                        break;
                    }
                    genre = genre->next;
                }

                if (!found)
                {
                    free(bookNode);
                }
            }
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
            if (memberNode != NULL)
            {
                result = insertMember(&library, memberNode);
            }
        }
    }
    else if (strncmp(data, "L ", 2) == 0)
    {
        int sid;
        int bid;
        if (sscanf(data, "L %d %d", &sid, &bid) == 2)
        {
            loan_t *loan = createLoan(sid, bid);
            if (loan != NULL)
            {
                member_t *current = library.members;
                /* Searches for member */
                while (current != NULL && current->sid != loan->sid)
                {
                    current = current->next;
                }

                /* Check if book exists in Library */
                int book_found = 0;
                genre_t *g_curr = library.genres;
                while (g_curr != NULL)
                {
                    book_t *b_curr = g_curr->books;
                    while (b_curr != NULL)
                    {
                        if (b_curr->bid == loan->bid)
                        {
                            book_found = 1;
                            break;
                        }
                        b_curr = b_curr->next;
                    }
                    if (book_found)
                        break;
                    g_curr = g_curr->next;
                }

                /* check if Member temp isnt NULL before inserting loan. */
                if (current != NULL && book_found)
                {
                    /* Check for duplicate loan */
                    loan_t *loan_curr = current->loans;
                    while (loan_curr != NULL)
                    {
                        if (loan_curr->bid == bid)
                        {
                            break;
                        }
                        loan_curr = loan_curr->next;
                    }
                    /* If duplicate found, free loan */
                    if (loan_curr != NULL && loan_curr->bid == bid)
                    {
                        free(loan);
                    }
                    else
                    {
                        /* Insert loan */
                        result = insertLoan(current, loan);
                    }
                }
                else
                {
                    free(loan);
                }
            }
        }
    }
    else if (strncmp(data, "R ", 2) == 0)
    {
        int sid;
        int bid;
        char score[8];
        char status[10];
        if (sscanf(data, "R %d %d %7s %9s", &sid, &bid, score, status) == 4)
        {
            /* find member by sid */
            member_t *member = library.members;
            while (member != NULL && member->sid != sid)
            {
                member = member->next;
            }
            if (member != NULL)
            {
                /* Check if loan is lost or returned */
                int lost_flag = -1;
                if (strcmp(status, "lost") == 0)
                {
                    lost_flag = 1;
                }
                else if (strcmp(status, "ok") == 0)
                {
                    lost_flag = 0;
                }
                /* Check if loan is lost or returned */
                if (lost_flag != -1)
                {
                    genre_t *g_curr = library.genres;
                    genre_t *found_genre = NULL;
                    book_t *book_curr = NULL;
                    int found_book = 0;

                    while (g_curr != NULL && found_book == 0)
                    {
                        book_curr = g_curr->books;
                        /* search for book in current genre */
                        while (book_curr != NULL)
                        {
                            if (book_curr->bid == bid)
                            {
                                /* If book is found, return loan */
                                found_genre = g_curr; /* save containing genre */
                                result = returnLoan(member, g_curr, book_curr, score, lost_flag);
                                found_book = 1;
                                break;
                            }
                            book_curr = book_curr->next;
                        }
                        if (!found_book)
                            g_curr = g_curr->next;
                    }

                    if (result == 0 && found_book == 1)
                    {
                        sortBook(found_genre, book_curr);
                    }
                }
            }
        }
    }
    else if (strncmp(data, "PG ", 3) == 0)
    {
        int gid;
        if (sscanf(data, "PG %d", &gid) == 1)
        {
            result = printGenre(&library, gid);
            
            /* printGenre doesnt print DONE on success */
            if (result == 0)
            {
                return;
            }
        }
    }
    else if (strncmp(data, "D ", 1) == 0)
    {
        result = allocateSlots();
    }
    else if (strncmp(data, "PD ", 2) == 0)
    {
        printDisplayedBooks();
        return;
    }
    else if (strncmp(data, "PM ", 3) == 0)
    {
        int sid;
        if (sscanf(data, "PM %d", &sid) == 1)
        {
            member_t *member = library.members;
            /* find member by sid */
            while (member != NULL && member->sid != sid)
                member = member->next;
            if (member == NULL)
            {
                printf("Loans:\n");
                return;
            }
            printMemberLoans(member);
            return;
        }
    }
    /* If event was not recognized or line is empty, exit function */
    else
    {
        return;
    }
    /* If event was successful, print DONE */
    if (result == 0)
    {
        printf("DONE\n");
        return;
    }
    printf("IGNORED\n");
}

/*
    Sets the total number of available display slots for the library.
    These get allocated later on for every genre respectively.
*/
int setSlots(int slots)
{
    SLOTS = slots;
    return 0;
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
int insertGenre(library_t *library, genre_t *genreNode)
{
    genre_t *tmp = library->genres;
    genre_t *prev = NULL;
    /* If the genre list is empty, make the new Genre it's head. */
    if (library->genres == NULL)
    {
        library->genres = genreNode;
        /* Returns 0 for DONE */
        return 0;
    }
    /* If list is not NULL and genreNode gid < head gid, make genreNode head of the list */
    if (genreNode->gid < tmp->gid)
    {
        genreNode->next = tmp;
        library->genres = genreNode;
        /* Returns 0 for DONE */
        return 0;
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
        /* Returns 1 for IGNORED */
        return 1;
    }

    /* If no duplicate gid is found, insert the genre to the singly linked list*/
    genreNode->next = tmp;
    if (prev != NULL)
    {
        prev->next = genreNode;
    }
    /* Returns 0 for DONE */
    return 0;
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
        return NULL;
    }
    memberNode->loans = NULL;
    memberNode->next = NULL;
    memberNode->sid = sid;
    strcpy(memberNode->name, name);
    return memberNode;
}

loan_t *createLoan(int sid, int bid)
{
    loan_t *loan = (loan_t *)malloc(sizeof(loan_t));
    if (loan == NULL)
    {
        printf("L IGNORED\n");
        return NULL;
    }
    loan->sid = sid;
    loan->bid = bid;
    return loan;
}

/* Helper Function to create sentinel node for linked lists that use it */
loan_t *createSentinelNode(int sid)
{
    loan_t *sentinel = (loan_t *)malloc(sizeof(loan_t));
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
int insertBook(genre_t *genre, book_t *bookNode)
{
    /* Check for duplicate bid */
    book_t *tmp = genre->books;
    while (tmp != NULL)
    {
        if (tmp->bid == bookNode->bid)
        {
            free(bookNode);
            return 1;
        }
        tmp = tmp->next;
    }

    /* Insert at head if books == NULL */
    if (genre->books == NULL)
    {
        genre->books = bookNode;
        bookNode->next = NULL;
        bookNode->prev = NULL;
        return 0;
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

    return 0;
}

/*
    Function to insert Member based on its ID into the member list.
    The insertion maintains the member list structure.
    The insertion is sorted based on mid (lowest to highest).
    Function runs on M event.
*/
int insertMember(library_t *library, member_t *member)
{

    member_t *current = library->members;
    member_t *prev = NULL;
    member->next = NULL;

    /* If the member list is empty, make the new Member it's head. */
    if (library->members == NULL)
    {
        library->members = member;
        return 0;
    }
    /* If list is not NULL and member sid < head sid, make genreNode head of the list */
    if (member->sid < current->sid)
    {
        member->next = current;
        library->members = member;
        return 0;
    }

    /* Traverse until second to last node to check for duplicates*/
    while (current != NULL && current->sid < member->sid)
    {
        prev = current;
        current = current->next;
    }

    /* Check last member node to ensure no duplicates*/
    if (current != NULL && current->sid == member->sid)
    {
        free(member);
        return 1;
    }

    /* If no duplicate gid is found, insert the member to the singly linked list*/
    member->next = current;
    if (prev != NULL)
    {
        prev->next = member;
    }
    return 0;
}

/*
    Function that inserts a new loan in the loans list.
    Runs on L event.
    Checks if head is empty sentinel and creates sentinel node and then inserts new loan.
    Else it simply inserts the new loan.
*/
int insertLoan(member_t *member, loan_t *loan)
{
    if (loan == NULL)
    {
        return 1;
    }

    /* If empty list create sentinel and insert */
    if (member->loans == NULL)
    {
        loan_t *sentinel = createSentinelNode(member->sid);
        if (sentinel == NULL)
        {
            free(loan);
            return 1;
        }
        loan->next = sentinel;
        member->loans = loan;
        return 0;
    }

    /* Insert at head */
    loan->next = member->loans;
    member->loans = loan;
    return 0;
}

/*
    Function to check if book list is sorted in a genre.
    prints every book's bid next to it's avg review score.
    Function runs on PG event.
*/
int printGenre(library_t *library, int gid)
{
    genre_t *tmp = library->genres;
    book_t *bTmp;
    if (tmp == NULL)
    {
        return -1;
    }
    /* Check if head is the genre */
    if (tmp->gid == gid)
    {
        /* Print books */
        bTmp = tmp->books;
        while (bTmp != NULL)
        {
            printf("%d %d\n", bTmp->bid, bTmp->avg);
            bTmp = bTmp->next;
        }
        return 0;
    }
    /* Traverse until genre is found */
    while (tmp != NULL && tmp->gid != gid)
    {
        tmp = tmp->next;
    }
    /* If genre is found, print books */
    if (tmp != NULL)
    {
        bTmp = tmp->books;
        while (bTmp != NULL)
        {
            printf("%d %d\n", bTmp->bid, bTmp->avg);
            bTmp = bTmp->next;
        }
        return 0;
    }
    /* If genre not found, return -1 */
    return -1;
}

/*
    Function to return a loan, update book review score and status.
    Runs on R event.
*/
int returnLoan(member_t *member, genre_t *genre, book_t *book, char *score, int status)
{
    int sc;
    /* Check score then convert to int */
    if (strcmp(score, "NA") == 0)
    {
        sc = -1;
    }
    else
    {
        /* Convert score to int */
        sc = atoi(score);
    }
    loan_t *curr = member->loans;
    loan_t *prev = NULL;

    /* If list empty or only sentinel */
    if (curr == NULL || curr->bid == -1)
    {
        return -1;
    }

    /* Traverse until matching bid or reach sentinel */
    while (curr->bid != -1 && curr->bid != book->bid)
    {
        prev = curr;
        curr = curr->next;
    }

    /* If we reached sentinel, book not found */
    if (curr->bid == -1)
    {
        return -1;
    }
    /* If found, apply new score and status to book */
    if (sc >= 0 && sc <= 10)
    {
        book->sum_scores += sc;
        book->n_reviews++;
        book->avg = (book->sum_scores / book->n_reviews); /* Calculate avg review score */
    }
    else
    {
        /* Invalid review score */
        genre->invalid_count++;
    }
    /* Set if book is ok or lost */
    book->lost_flag = status;
    if (book->lost_flag == 1)
    {
        /* Increment lost count for genre */
        genre->lost_count++;
    }

    /* delete node if found */
    if (prev == NULL)
    {
        /* Node to delete is first loan */
        member->loans = curr->next;
    }
    else
    {
        prev->next = curr->next;
    }
    /* Free the deleted loan node */
    free(curr);
    return 0;
}

/*
    Function to sort book in genre's book list after its avg review score has been updated.
    The sorting is done in descending order of avg review score.
    If two books have the same avg review score, they are sorted by ascending order of bid.
*/
void sortBook(genre_t *g, book_t *book)
{
    if (g == NULL || book == NULL)
        return;

    /* Splice out book from current list */
    if (book->prev)
    {
        book->prev->next = book->next;
    }
    else if (g->books == book)
    {
        g->books = book->next;
    }
    if (book->next)
    {
        book->next->prev = book->prev;
    }
    /* Clear book links so insertion is clean */
    book->prev = NULL;
    book->next = NULL;

    /* Find insertion point */
    book_t *cur = g->books;
    book_t *prev = NULL;
    while (cur && (cur->avg > book->avg || (cur->avg == book->avg && cur->bid < book->bid)))
    {
        prev = cur;
        cur = cur->next;
    }

    /* Splice in book at found position */
    if (prev == NULL)
    {
        /* insert at head */
        book->next = g->books;
        if (g->books)
            g->books->prev = book;
        g->books = book;
        book->prev = NULL;
    }
    else
    {
        /* insert after prev */
        book->prev = prev;
        book->next = cur;
        prev->next = book;
        if (cur)
        {
            cur->prev = book;
        }
    }
}
/* Function to print all loans of a member */
void printMemberLoans(member_t *member)
{
    loan_t *loan = member->loans;
    if (loan == NULL || loan->bid == -1)
    {
        printf("Loans:\n");
        return;
    }
    printf("Loans:\n");
    while (loan->bid != -1)
    {
        printf("%d\n", loan->bid);
        loan = loan->next;
    }
}

/*  Using the points helper function, allocate slots for every genre based on their points.
    Function runs on D event. */
int allocateSlots()
{
    int VALID = 0;
    int QUOTA = 0;
    int slots_used = 0;
    genre_t *tmp = library.genres;

    /* Check if any genres exist */
    if (tmp == NULL)
    {
        return -1;
    }

    /* If no slots, set all genre slots to 0 and return */
    if (SLOTS == 0)
    {
        while (tmp != NULL)
        {
            tmp->slots = 0;
            tmp = tmp->next;
        }
        return 0;
    }
    /* Calculate total valid points */
    tmp = library.genres;
    while (tmp != NULL)
    {
        // Call points() ONCE and store the result
        tmp->points = points(tmp);
        VALID += tmp->points;
        tmp = tmp->next;
    }
    /* If no valid points, set all genre slots to 0 and return */
    if (VALID == 0)
    {
        tmp = library.genres;
        while (tmp != NULL)
        {
            tmp->slots = 0;
            tmp = tmp->next;
        }
        return 0;
    }
    /* Calculate quota */
    QUOTA = VALID / SLOTS;

    /* Allocate initial seats based on quota */
    slots_used = 0;
    tmp = library.genres;
    while (tmp != NULL)
    {
          if (QUOTA > 0)
          {
             /* Allocate seats */
             {
                int pts = tmp->points;
                int g_seats = seats(tmp, pts, QUOTA);
                tmp->slots = g_seats;
                slots_used += g_seats;
                tmp->remainder = rem(tmp, pts, QUOTA, g_seats);
            }
        }
        else
        {
            tmp->slots = 0;
            tmp->remainder = tmp->points;
        }
        tmp = tmp->next;
    }
    /* Calculate remaining seats */
    int remaining = SLOTS - slots_used;
    while (remaining > 0)
    {
        genre_t *best = NULL;
        int max_rem = -1;

        tmp = library.genres;
        while (tmp != NULL)
        {
            int g_rem = tmp->remainder;
            if (g_rem != -1)
            {
                if (best == NULL || g_rem > max_rem || (g_rem == max_rem && tmp->gid < best->gid))
                {
                    best = tmp;
                    max_rem = g_rem;
                }
            }
            tmp = tmp->next;
        }

        if (best == NULL)
        {
            break;
        }

        best->slots += 1;
        /* Mark as used */
        best->remainder = -1;
        remaining--;
    }

    /* Choose top books of genre */
    tmp = library.genres;
    while (tmp != NULL)
    {
        /* Free previous display */
        if (tmp->display != NULL)
        {
            free(tmp->display);
            tmp->display = NULL;
        }

        if (tmp->slots > 0)
        {
            tmp->display = malloc(sizeof(book_t *) * tmp->slots);
            if (tmp->display == NULL)
            {
                return -1;
            }

            /* Fill display array with top books */
            book_t **curr_disp = tmp->display;
            book_t *book = tmp->books;
            int seats_left = tmp->slots;

            while (book != NULL && seats_left > 0)
            {
                if (book->lost_flag == 0)
                {
                    /* Save pointer to the book */
                    *curr_disp = book; 
                    /* Move pointer ahead */
                    curr_disp++;       
                    /* Decrease seats left */
                    seats_left--;
                }
                book = book->next;
            }

            /* If not enough valid books, fill remaining slots with NULL */
            while (seats_left > 0)
            {
                *curr_disp = NULL;
                curr_disp++;
                seats_left--;
            }
        }
        tmp = tmp->next;
    }
    return 0;
}

/* CALCULATE GENRE POINTS */
int points(genre_t *g)
{
    int points = 0;
    book_t *tmp = g->books;
    if (tmp == NULL)
    {
        return 0;
    }
    while (tmp != NULL)
    {
        if (tmp->n_reviews > 0 && tmp->lost_flag == 0)
        {
            points += tmp->sum_scores;
        }
        tmp = tmp->next;
    }
    return points;
}

/* Calculate genre seats */
int seats(genre_t *g, int points, int quota)
{
    if (quota == 0)
    {
        return 0;
    }
    int seats;
    seats = points / quota;
    return seats;
}

/* Calculate remainder */
int rem(genre_t *g, int points, int quota, int seat)
{
    int remainder = points - (seat * quota);
    return remainder;
}

/*
    Function that prints all books in the display list.
    Runs on PD event.
*/
void printDisplayedBooks()
{
    printf("Display\n");
    genre_t *tmp = library.genres;
    
    /* Check if any genres exist or if the library has any slots available */
    if (tmp == NULL || SLOTS == 0)
    {
        printf("(empty)\n");
        return;
    }

    int display_exists = 0;

    /* Traverse genres */
    while (tmp != NULL)
    {
        if (tmp->slots > 0 && tmp->display != NULL)
        {
            /* At least one display exists */
            display_exists = 1;
            printf("%d:\n", tmp->gid);
            
            /* Print displayed books */
            int seats_left = tmp->slots;
            book_t **display_tmp = tmp->display;

            /* Traverse displayed list */
            while (seats_left > 0)
            {
                book_t *b = *display_tmp;
                if (b != NULL)
                {
                    printf("%d, %d\n", b->bid, b->avg);
                }
                display_tmp++;
                seats_left--;
            }
        }
        tmp = tmp->next;
    }

    /* If no display exists, print (empty) */
    if (!display_exists)
    {
        printf("(empty)\n");
    }
}

/* Function that creates a new BookNode node for the AVL tree */
BookNode* MakeNewBookNode(book_t *book)
{
    BookNode* node = malloc(sizeof(BookNode));
    if(!node) return NULL;
    strcpy(node->title,book->title);
    node->book = book;
    node->height =  1;
    node->rc = node->lc = NULL;
    return node;
}

BookNode *AVLLookUp(char* key, BookNode *book)
{
    if (!book) return NULL;
    if (strcmp(key,book->title) == 0) return book;
    if (strcmp(key,book->title) < 0) return AVLLookUp(key, book->lc);
    return AVLLookUp(key, book->rc);
}

BookNode* LeftRotate(BookNode* x)
{
    BookNode* rightChild = x->rc;
    BookNode* LeftChildR = rightChild->lc; /* Left Child of rightChild of x */

    /* Rotation */
    rightChild->lc = x; /* Set left child of right child of x x itself. */
    x->rc = LeftChildR; /* set the left child of the right child of righChild of x as leftchild of x */

    x->height = max_height(height(x->lc), height(x->rc)) + 1;
    
    /* New root of subtree */
    rightChild->height = max_height(height(rightChild->lc), height(rightChild->rc)) + 1;

    return rightChild;
}

BookNode* RightRotate(BookNode* x)
{
    BookNode* leftChild = x->lc;
    BookNode* rightChildL = leftChild->rc; /* Right child of leftChild of x */

    /* Rotation */
    leftChild->rc = x; /* Pivot leftChild so its right child becomes x */
    x->lc = rightChildL; /* Move right child of leftChild to left subtree of x */

    x->height = max_height(height(x->lc), height(x->rc)) + 1;
    
    /* New root of subtree */
    leftChild->height = max_height(height(leftChild->lc), height(leftChild->rc)) + 1;

    return leftChild;
}

int height(BookNode* n)
{
    if (!n) return 0;
    return n->height;
}

int max_height(int x, int y)
{
    if (x>y) return x;
    else return y;
}

int get_balance(BookNode *n) {
    if (!n) return 0;
    return height(n->lc) - height(n->rc);
}

void PreOrder(BookNode *root)
{
    if (!root) return;
    Visit(root);
    PreOrder(root->left);
    PreOrder(root->right);
}

void InOrder(BookNode *root)
{
    if (!root)
        return;
    InOrder(root->left);
    Visit(root);
    InOrder(root->right);
}

void Visit(BookNode *book)
{
    printf("Key: %s, Data: %s\n", book->title, book->book->title);
}