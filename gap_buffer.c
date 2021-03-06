/*
 * Implementation of Gap Buffer Text Storage, adapted for multiple cursors.
 *
 * AUTHORS: ETHAN CHENG <elc1798>, SOPHIA ZHENG <sophz55>
 *
 * ANNOTATED/EDITED/DEBUGGED BY: YICHENG WANG <alex-wyc>
 *
 * Theory:
 *
 *  In a traditional gap buffer, we can store a string in the fashion:
 *  ['h', 'e', 'l', 'l', 'o', '\n' , 'w', 0, 0, 0, 0, 0, 'o', 'r', 'l', 'd']
 *  This displays the:
 *  hello
 *  world
 *  With the cursor over the 'o' in "world". If the 'p' character is entered,
 *  the closest NULL byte to 'o' on its left will be turned into 'p', resulting
 *  in the buffer:
 *  ['h', 'e', 'l', 'l', 'o', '\n' , 'w', 0, 0, 0, 0, 'p', 'o', 'r', 'l', 'd']
 *
 *  However, this poses limitations, as you cannot differentiate between
 *  cursors. We can instead make the buffer a 2D array. Each element will be in
 *  the format: [CHARACTER, OWNER_ID], where the data type of this gap buffer
 *  will be:
 *
 *  unsigned char[128][2] buffer;
 *
 *  Using unsigned char allows us to maintain the character set, as well as do
 *  some clever encoding:
 *
 *      We can limit the number of editors per room to 8, a reasonable number.
 *      Storing the editors (users) in an array, indices 0-7, denoted as 'i',
 *      we can mark down the location of a cursor by flipping the i-th bit of the
 *      OWNER_ID (buffer[n][1] for the nth character) to true.
 *
 *      Ex:
 *          Users: [ASH, BOB, CRIS, DAVID, ETHAN, FRITZ, GERALD, 0] (0 denotes
 *          an empty slot)
 *
 *          An element of buffer, [0, 00001011] denotes a cursor location, since
 *          the index 1 is not a NULL byte. The cursors of ASH, BOB, and DAVID all
 *          overlap on this position, since the 0th, 1st, and 3rd bits are
 *          flipped.
 *
 * These gap buffers are stored in a linked list of gap buffers, so we can
 * easily string them together, and extend the range of the gap buffers.
 */

/* IMPLEMENTATION */

#include "gap_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/*
 * Checks for certain rules that define a valid buffer
 */
bool is_gapbuf(gapbuf G) {
    bool valid = (G != NULL && G->limit > 0); // pointer isn't null and limit > 0
    int i;

    for (i = 0; valid && i < MAX_USERS; i++) { // while it is valid
        valid = valid &&
                G->gap_start[i] >= 0 && // start cursor for user not negative
                G->gap_start[i] <= G->gap_end[i] && // start cursor for user less than end cursor for user
                G->gap_end[i] <= G->limit; // end cursor still within the gap
    }
    return valid;
}

/*
 * Checks if the gap buffer is empty for all users
 */
bool gapbuf_empty(gapbuf G) {
    assert(is_gapbuf(G));
    bool empty = true;
    int i;

    for (i = 0; empty && i < MAX_USERS; i++) {
        empty = empty &&
                G->gap_start[i] == 0 && // no character at start
                G->gap_end[i] == G->limit; // no character at the end
    }
    return empty;
}

/*
 * Checks if the gap buffer is full for any user
 */
bool gapbuf_full(gapbuf G) {
    assert(is_gapbuf(G));
    bool full = false;
    int i;

    for (i = 0; !full && i < MAX_USERS; i++) {
        full = full ||
               (G->gap_start[i] == G->gap_end[i]); // if the beginning of the gap and the end of the gap are the same, then the buffer is full
    }
    return full;
}

/*
 * Checks if 'user' has a gap at the left of G
 */
bool gap_at_left(gapbuf G, int user) {
    assert(is_gapbuf(G));
    assert(0 <= user && user < MAX_USERS);
    return G->gap_start[user] == 0;
}

/*
 * Checks if 'user' has a gap at the right of G
 */
bool gap_at_right(gapbuf G, int user) {
    assert(is_gapbuf(G));
    assert(0 <= user && user < MAX_USERS);
    return G->gap_end[user] == G->limit;
}

/*
 * Returns a new gapbuffer with specified character limit
 */
gapbuf new_gapbuf(int limit) {
    assert(limit > 0);
    gapbuf G = (gapbuf)malloc(sizeof(struct gap_buffer));
    G->limit = limit;
    G->buffer = calloc(limit, sizeof(char*));
    int i;

    for (i = 0; i < limit; i++) {
        G->buffer[i] = malloc(2 * sizeof(char)); // two bytes, [char, user-bit-switch]
        printf("Allocated memory for buffer elem\n");
        G->buffer[i][0] = 0;
        printf("Set initial value of %d to 0\n", i);
        G->buffer[i][1] = ~0; // 11111111 in binary
        // initially everyone is on the gap
        printf("Set initial cursor value of %d to %x\n", i, ~0);
    }

    for (i = 0; i < MAX_USERS; i++) {
        G->gap_start[i] = 0;
        G->gap_end[i] = limit;
    }

    return G;
}

void remove_user_from(gapbuf G, int user, int index) { // check
    G->buffer[index][1] = G->buffer[index][1] & ~(1 << user);
}

void add_user_to(gapbuf G, int user, int index) { // check
    G->buffer[index][1] = G->buffer[index][1] | (1 << user);
}

/*
 * Returns true if user has cursor object in index
 */
bool is_user_in(gapbuf G, int index, int user) { // check
    return (G->buffer[index][1] >> user) & 0x01;
}

/*
 * Moves gapbuf for user forward
 */
void gapbuf_forward(gapbuf G, int user) {
    assert(is_gapbuf(G));
    assert(0 <= user && user < MAX_USERS);

    remove_user_from(G, user, G->gap_start[user]);
    G->gap_start[user]++;
    G->gap_end[user]++;
    add_user_to(G, user, G->gap_end[user]);
}

/*
 * Moves gapbuf for user backward
 */
void gapbuf_backward(gapbuf G, int user){
    assert(is_gapbuf(G));
    assert(0 <= user && user < MAX_USERS);

    G->gap_start[user]--;
    add_user_to(G, user, G->gap_start[user]);
    remove_user_from(G, user, G->gap_end[user]);
    G->gap_end[user]--;
}

/*
 * Inserts char c before the gap for user
 */
void gapbuf_insert(gapbuf G, char c, int user){
    assert(is_gapbuf(G));
    assert(0 <= user && user < MAX_USERS);

    G->buffer[G->gap_start[user]][0] = c; // fill in the gap
    remove_user_from(G, user, G->gap_start[user]); // lessen the gap for user
    G->gap_start[user]++; // move the beginning forward
}

/*
 * Deletes the character before the gap for user
 */
void gapbuf_delete(gapbuf G, int user){
    assert(is_gapbuf(G));
    assert(0 <= user && user < MAX_USERS);

    G->buffer[G->gap_start[user]][0] = 0;
    G->gap_start[user]--;
    add_user_to(G, user, G->gap_end[user]);
}

/*
 * Frees the memory of a gap buffer, and returns a NULL pointer
 */
gapbuf free_gapbuf(gapbuf G) {
    assert(is_gapbuf(G));
    int i;
    for (i = 0; i < G->limit; i++) {
        free(G->buffer[i]);
        G->buffer[i] = NULL;
    }
    free(G->buffer);
    G->buffer = NULL;
    free(G);
    // G = NULL; <-- cannot just do that lol... pass by value much
    return NULL;
}
