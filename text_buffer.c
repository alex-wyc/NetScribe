/*
 * Text Buffer : A doubly linked list of nodes, containing gap buffers. This
 * primes our gap buffer implementation for use in a text editor.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "gap_buffer.h"
#include "text_buffer.h"

/*
 * Checks if text buffer is a well-formed doubly linked list
 */
bool is_valid_tbuf(tbuf tb) {
    if (tb == NULL || tb->start == NULL || tb->end == NULL || tb->start == tb->end) {
        return false;
    }
    bool has_current = false;
    dll temp = tb->start;
    while (temp->next != NULL && temp->next != tb->end) {
        if (temp != temp->next->prev) return false; // lmao pls
        if (temp == tb->current) has_current = true; // Check for existence of 'current'
        temp = temp->next;                          // Iterate
    }
    return has_current && temp->next != NULL && temp->next->prev == temp;
}

/*
 * Returns true if the point is the first (non-terminal) node of the list.
 * Requires that the tbuf is valid
 */
bool tbuf_current_at_start(tbuf tb) {
    assert(is_valid_tbuf(tb));
    return tb->current == tb->start->next;
}

/*
 * Returns true if the point is the last (non-terminal) node of the list.
 * Requires that the tbuf is valid
 */
bool tbuf_current_at_end(tbuf tb) {
    assert(is_valid_tbuf(tb));
    return tb->end == tb->current->next;
}

/*
 * Moves the current forward, towards the end. Requires valid tbuf, and that
 * current is NOT AT END. Maintains the validity of tbuf and ensures that the
 * current is NOT AT START after completion.
 */
void tbuf_forward(tbuf tb) {
    assert(!tbuf_current_at_end(tb));
    tb->current = tb->current->next;
    assert(!tbuf_current_at_start(tb));
}

/*
 * Moves the current backward, towards start. Requires valid tbuf, and that
 * current is NOT AT START. Maintains the validity of tbuf and ensures that the
 * current IS NOT AT END after completion.
 */
void tbuf_backward(tbuf tb) {
    assert(!tbuf_current_at_start(tb));
    tb->current = tb->current->prev;
    assert(!tbuf_current_at_end(tb));
}

/*
 * Removes the current node from the text buffer. Moves the current to the next
 * node, unless it was already at the right-most (last / end) node, in which
 * case, it would move to the previous one.
 * Requires that tb is a valid textbuf, and that the current node is not the
 * only node in the textbuf.
 * Ensures that the tb is still a valid textbuf after completion.
 */
void tbuf_delete_current(tbuf tb) {
    assert(!tbuf_current_at_start(tb) || !tbuf_current_at_end(tb));
    tb->current->prev->next = tb->current->next;
    tb->current->next->prev = tb->current->prev;
    dll to_free = tb->current;
    tb->current = tb->current->next;
    if (tb->current == tb->end) tb->current = tb->current->prev;

    free_gapbuf(to_free->data);
    free(to_free);
    to_free = NULL;
}

/*
 * Frees the memory associated with tb and sets tb to NULL
 */
void free_tbuf(tbuf tb) {
    tb->current = tb->start->next;
    while(tb->current->next != tb->end) {
        tbuf_delete_current(tb);
    }
    free_gapbuf(tb->current->data);
    free(tb->start);
    free(tb->current);
    free(tb->end);
    tb->start = NULL;
    tb->current = NULL;
    tb->end = NULL;
}

