/*
 * Text Buffer : A doubly linked list of nodes, containing gap buffers. This
 * primes our gap buffer implementation for use in a text editor.
 *
 * To specify which user, the user will be passed as a parameter.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "gap_buffer.h"
#include "text_buffer.h"
#include "editor_backend.h"

/*
 * Checks if text buffer is a well-formed doubly linked list
 */
bool is_valid_tbuf(tbuf tb) {
    if (tb == NULL || tb->start == NULL || tb->end == NULL || tb->start == tb->end) {
        printf("Non NULL data ASSERTION FAILED\n");
        return false;
    }
    bool has_current[MAX_USERS];
    int i; for (i = 0; i < MAX_USERS; i++) {
        has_current[i] = false;
    }

    dll temp = tb->start;
    while (temp->next != NULL && temp->next != tb->end) {
        if (temp != temp->next->prev) {
            printf("Doubly linked-ness ASSERTION FAILED\n");
            return false; // lmao pls
        }
        for (i = 0; i < MAX_USERS; i++) {
            if (temp == tb->current[i]) has_current[i] = true;
        }
        temp = temp->next;
    }

    bool has_all_currents = true;
    for (i = 0; has_all_currents && i < MAX_USERS; i++) {
        has_all_currents = has_all_currents && has_current[i];
        if (!has_all_currents) {
            printf("Gapbuf user %d does not have a current node\n", i);
        }
    }

    return has_all_currents && temp->next != NULL && temp->next->prev == temp;
}

/*
 * Returns true if the user's current node is the first (non-terminal) node of
 * the list. Requires that the tbuf is valid
 */
bool tbuf_current_at_start(tbuf tb, int user) {
    assert(is_valid_tbuf(tb));
    assert(0 <= user && user < 8);
    return tb->current[user] == tb->start->next;
}

/*
 * Returns true if the user's current node is the last (non-terminal) node of
 * the list. Requires that the tbuf is valid
 */
bool tbuf_current_at_end(tbuf tb, int user) {
    assert(is_valid_tbuf(tb));
    assert(0 <= user && user < 8);
    return tb->end == tb->current[user]->next;
}

/*
 * Moves the current of user forward, towards the end. Requires valid tbuf, and
 * that current is NOT AT END. Maintains the validity of tbuf and ensures that
 * the current is NOT AT START after completion.
 */
void tbuf_forward(tbuf tb, int user) {
    assert(0 <= user && user < 8);
    assert(!tbuf_current_at_end(tb, user));
    tb->current[user] = tb->current[user]->next;
    assert(!tbuf_current_at_start(tb, user));
}

/*
 * Moves the current of user backward, towards start. Requires valid tbuf, and
 * that current is NOT AT START. Maintains the validity of tbuf and ensures that
 * the current IS NOT AT END after completion.
 */
void tbuf_backward(tbuf tb, int user) {
    assert(0 <= user && user < 8);
    assert(!tbuf_current_at_start(tb, user));
    tb->current[user] = tb->current[user]->prev;
    assert(!tbuf_current_at_end(tb, user));
}

/*
 * Removes the current node of user from the text buffer. Moves the current to
 * the next node, unless it was already at the right-most (last / end) node, in
 * which case, it would move to the previous one.
 * Requires that tb is a valid textbuf, and that the current node is not the
 * only node in the textbuf.
 * Ensures that the tb is still a valid textbuf after completion.
 */
void tbuf_delete_current(tbuf tb, int user) {
    assert(0 <= user && user < 8);
    assert(!tbuf_current_at_start(tb, user) || !tbuf_current_at_end(tb, user));

    dll to_free = tb->current[user];

    int i; for (i = 0; i < MAX_USERS; i++) {
        if (tb->current[i] != to_free) {
            continue;
        }
        tb->current[i]->prev->next = tb->current[i]->next;
        tb->current[i]->next->prev = tb->current[i]->prev;
        tb->current[i] = tb->current[i]->next;
        if (tb->current[i] == tb->end) {
            tb->current[i] = tb->current[i]->prev;
        }
    }

    free_gapbuf(to_free->data);
    free(to_free);
    to_free = NULL;
}

/*
 * Frees the memory associated with tb and sets tb to NULL
 */
void free_tbuf(tbuf tb) {
    tb->current[0] = tb->start->next;
    while(tb->current[0]->next != tb->end) {
        tbuf_delete_current(tb, 0);
    }
    free_gapbuf(tb->current[0]->data);
    free(tb->start);
    tb->start = NULL;
    int i; for (i = 0; i < MAX_USERS; i++) {
        free(tb->current[i]);
        tb->current[i] = NULL;
    }
    free(tb->end);
    tb->end = NULL;
}

char *tbuf2chararr(tbuf tb) {
    char *buff = calloc(20480, sizeof(char));
    int count = 0;
    int i;
    dll tmp = tb->start->next;
    while (tmp != tb->end) {
        for (i = 0; i < 16; i++) {
            if (tmp->data->buffer[i][0]) {
                buff[count++] = tmp->data->buffer[i][0];
            }
        }
        tmp = tmp->next;
    }
    buff[count] = '\0';
    return buff;
}

tbuf chararr2tbuf(char *s) {
    tbuf retval = new_tbuf();
    int i = 0;
    while (s[i]) {
        insert_char(retval, s[i], 0);
    }
    retval->current[0] = retval->current[1];
    return retval;
}

