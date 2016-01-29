#include <stdbool.h>
#include "gap_buffer.h"

#ifndef _TEXT_BUFFER
#define _TEXT_BUFFER

struct list_node {
    gapbuf data;    /* Gap Buffer of 16 Bytes */
    struct list_node *next;
    struct list_node *prev;
};

typedef struct list_node* dll; // dll = doubly linked list

struct text_buffer {
    dll start;              /* First node, terminal    */
    dll current[MAX_USERS]; /* Current nodes for users */
    dll end;                /* Last node, terminal     */
};

typedef struct text_buffer* tbuf;

// Yes I am aware that tb is tuberculosis
bool is_valid_tbuf(tbuf tb);                    // Checks if tbuf is well formed dll
bool tbuf_current_at_start(tbuf tb, int user);  // Checks if current is at start
bool tbuf_current_at_end(tbuf tb, int user);    // Checks if current is at end
void tbuf_forward(tbuf tb, int user);           // Moves the current to the right (end)
void tbuf_backward(tbuf tb, int user);          // Moves the current to the left (start)
void tbuf_delete_current(tbuf tb, int user);    // Removes the current node from list
void free_tbuf(tbuf tb);                        // Frees all memory associated with a single tbuf node
char *tbuf2chararr(tbuf tb);                    // Converts a tbuf to char*
tbuf chararr2tbuf(char *s);                     // Converts a char* to tbuf
#endif
