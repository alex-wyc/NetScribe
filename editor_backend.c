/*
 * Backend for single-character, multi-cursor text editor, using doubly-linked
 * lists of 16 character gap buffers.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "gap_buffer.h"
#include "text_buffer.h"

/*
 * Checks if a text buffer is well formed, consists of only buffers of size 16,
 * and each user has either one empty gap buffer or one or more non-empy gap
 * buffers, and gap buffers are aligned.
 */
bool is_tbuf(tbuf tb) {
    if (!is_valid_tbuf(tb)) {
        return false;
    }
    bool is_empty = false;
    int curr_locs[MAX_USERS];
    int i; for (i = 0; i < MAX_USERS; i++) {
        curr_locs[i] = INT32_MAX;
    }
    int count = 0;
    dll tmp = tb->start->next;

    while (tmp != tb->end) {
        count++;

        if (!is_gapbuf(tmp->data)) return false;
        if (tmp->data->limit != 16) return false;

        is_empty = is_empty || gapbuf_empty(tmp->data);

        for (i = 0; i < MAX_USERS; i++) {
            if (tmp == tb->current[i]) {
                curr_locs[i] = count;
            }
            if (count < curr_locs[i] && !gap_at_right(tmp->data, i)) {
                return false;
            }
            if (count > curr_locs[i] && !gap_at_left(tmp->data, i)) {
                return false;
            }
        }

        tmp = tmp->next;
    }

    if (is_empty && count != 1) {
        return false;
    }

    return true;
}

/*
 * Checks if the text buffer is empty. Requires that tb is a tbuf.
 */
bool tbuf_empty(tbuf tb) {
    assert(is_tbuf(tb));
    bool retval = true;
    int i; for (i = 0; i < MAX_USERS; i++) {
        retval = retval && gapbuf_empty(tb->current[i]->data);
    }; 
    return retval;
}

/*
 * Creates an empty text buffer, with gap buffers of size 16 bytes
 */
tbuf new_tbuf() {
    tbuf retval = malloc(sizeof(struct text_buffer));
    retval->start = malloc(sizeof(struct list_node));
    retval->end = malloc(sizeof(struct list_node));
    dll FIRST_BLOCK = malloc(sizeof(struct list_node));
    int i; for (i = 0; i < MAX_USERS; i++) {
        retval->current[i] = FIRST_BLOCK;
    }
    FIRST_BLOCK->data = new_gapbuf(16);

    retval->start->next = FIRST_BLOCK;
    FIRST_BLOCK->next = retval->end;
    retval->end->prev = FIRST_BLOCK;
    FIRST_BLOCK->prev = retval->start;

    return retval;
}

/*
 * Splits a full tbuf node into two nodes, so that the current node is not full.
 */
void split_point(tbuf tb, int user) {
    assert(is_tbuf(tb));
    assert(gapbuf_full(tb->current[user]->data));
    dll new_node = malloc(sizeof(struct list_node));
    new_node->data = new_gapbuf(16);
    int i; for (i = 0; i < 16; i++) {
        // We can copy the entire array [CHARACTER, IDS] in one line :D
        new_node->data->buffer[i] = tb->current[user]->data->buffer[i];
    }
    if (tb->current[user]->data->gap_start[user] < 8) {
        new_node->next = tb->current[user]->next;
        new_node->prev = tb->current[user];
        tb->current[user]->next = new_node;
        new_node->next->prev = new_node;

        new_node->data->gap_end[user] = 8;

        for (i = tb->current[user]->data->gap_start[user]; i < 8; i++) {
            tb->current[user]->data->buffer[i+8] = tb->current[user]->data->buffer[i];
        }

        tb->current[user]->data->gap_end[user] += 8;
    } else {
        new_node->prev = tb->current[user]->prev;
        new_node->next = tb->current[user];
        tb->current[user]->prev = new_node;
        new_node->prev->next = new_node;

        new_node->data->gap_start[user] = 8;

        for (i = 8; i < tb->current[user]->data->gap_start[user]; i++) {
            tb->current[user]->data->buffer[i - 8] = tb->current[user]->data->buffer[i];
        }
        tb->current[user]->data->gap_start[user] -= 8;
    }
}
 
/*
 * Move the cursor forward 1 char, to the right. Does nothing if the cursor is
 * all the way right. Requires and that tb is valid.
 */
void forward_char(tbuf tb, int user) {
    assert(is_tbuf(tb));
    if (gap_at_right(tb->current[user]->data, user)) {
        if (tbuf_current_at_end(tb, user)) {
            return;
        }
        tbuf_forward(tb, user);
    }
    gapbuf_forward(tb->current[user]->data, user);
    assert(is_tbuf(tb));
}

/*
 * Move the cursor backward 1 char, to the left. Does nothing if the cursor is
 * all the way left. Requires that the tb is valid.
 */
void backward_char(tbuf tb, int user) {
    assert(is_tbuf(tb));
    if (gap_at_left(tb->current[user]->data, user)) {
        if (tbuf_current_at_start(tb, user)) {
            return;
        }
        tbuf_backward(tb, user);
    }
    gapbuf_backward(tb->current[user]->data, user);
    assert(is_tbuf(tb));
}

/*
 * Insert the char c before the cursor. Splits the current if the current is
 * full. tb must be valid. Ensures that tb is valid after completion, and that
 * the char before gap_start of the gapbuf at the point is equal to c
 */
void insert_char(tbuf tb, char c, int user) {
    assert(is_tbuf(tb));
    if (gapbuf_full(tb->current[user]->data)) {
        split_point(tb, user);
    }
    gapbuf_insert(tb->current[user]->data, c, user);
    assert(is_tbuf(tb));
}

/*
 * Delete the character before the cursor, also deletes the current if it is
 * empty. Requires tb to be valid, ensures that tb is still valid after
 * completion.
 */
void delete_char(tbuf tb, int user) {
    assert(is_tbuf(tb));
    if (gap_at_left(tb->current[user]->data, user)) {
        if (tbuf_current_at_start(tb, user)) {
            return;
        }
        tbuf_backward(tb, user);
    }

    gapbuf_delete(tb->current[user]->data, user);
    if (gapbuf_empty(tb->current[user]->data) && !(tb->start->next->next == tb->end)) {
        tbuf_delete_current(tb, user);
    }
    assert(is_tbuf(tb));
}

