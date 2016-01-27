#include <stdbool.h>
#include "gap_buffer.h"

#ifndef _TEXT_BUFFER
#define _TEXT_BUFFER

typedef struct list_node* dll; // dll = doubly linked list
struct list_node {
    gapbuf data;    /* Gap Buffer of 16 Bytes */
    dll next;
    dll prev;
};

typedef struct text_buffer* tbuf;
struct text_buffer {
    dll start;              /* First node, terminal    */
    dll current[MAX_USERS]; /* Current nodes for users */
    dll end;                /* Last node, terminal     */
};

// Yes I am aware that tb is tuberculosis
bool is_valid_tbuf(tbuf tb);                    // Checks if tbuf is well formed dll
bool tbuf_current_at_start(tbuf tb, int user);  // Checks if current is at start
bool tbuf_current_at_end(tbuf tb, int user);    // Checks if current is at end
void tbuf_forward(tbuf tb, int user);           // Moves the current to the right (end)
void tbuf_backward(tbuf tb, int user);          // Moves the current to the left (start)
void tbuf_delete_current(tbuf tb, int user);    // Removes the current node from list
void free_tbuf(tbuf tb);                        // Frees all memory associated with a single tbuf node
#endif
