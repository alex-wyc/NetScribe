/*******************************************************************************
 * Hacky text editor front end using curses                                    *
 *                                                                             *
 * Authors                                                                     *
 *  Yicheng Wang <alex-wyc>                                                    *
 *                                                                             *
 * Description                                                                 *
 *  ayyy curses                                                                *
 *                                                                             *
 ******************************************************************************/

/* TODO
 *  TODO-List
 */

/* Dev Log
 *  Project Created: 2016-01-27 17:45 - Yicheng W.
 */

#include <stdio.h>
#include <curses.h>
#include <string.h>

#include "gap_buffer.h"
#include "text_buffer.h"
#include "editor_backend.h"

int USER_ME; // this is the local index of me

/* render_char: renders the character, and if the pointer points to the
 * character it will be hihglighted. Once it is printed, the pointer will be set
 * to false, so only one character will ever be highlighted
 */
void render_char (char c, bool *point, WINDOW *w){
    if (point != NULL && *point) {
        if (c == '\n') {
            waddch(w, ' ' | A_STANDOUT);
            waddch(w, '\n');
        }
        else {
            waddch(w, c | A_STANDOUT);
        }
        *point = false;
    }
    else {
        waddch(w, c);
    }
}

/* render_gapbuf: renders and entire gap buffer
 * TODO long description
 */
void render_gapbuf (gapbuf G, bool *point, WINDOW *w){
    int i;
    for (i = 0 ; i < G->)
}
