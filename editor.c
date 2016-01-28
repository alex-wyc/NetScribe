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
#include <socket.h>

#include "gap_buffer.h"
#include "text_buffer.h"
#include "editor_backend.h"
#include "central.h"

/* server side definitions */
#define SERVER_ADDR "127.0.0.1"

/* ncurses definitions/macro hacks */
#define getbegx(_win, _x) { \
    int _y;                 \
    getbegxy(_win, _x, _y); \
}

#define getmaxx(_win, _x) { \
    int _y;                 \
    getmaxxy(_win, _x, _y); \
}

#define highlight(x) x | A_STANDOUT


int USER_ME; // this is the local index of me

/* render_char: renders the character, and if the pointer points to the
 * character it will be hihglighted
 */
void render_char (char c, bool point, WINDOW *w){
    if (point) {
        if (c == '\n') {
            waddch(w, highlight(c));
            waddch(w, '\n');
        }
        else {
            waddch(w, highlight(c));
        }
    }
    else {
        waddch(w, c);
    }
}

/* render_gapbuf: renders an entire gap buffer
 */
void render_gapbuf (gapbuf G, WINDOW *w){
    int i;
    for (i = 0 ; i < G->limit; i++) {
        // If G->buffer[i][1] != 0 , print G->buffer[i][0]
        // else print that highlighted
        if (G->buffer[i][1] == 0) { // if highlighted
            render_char(G->buffer[i][0], true, w);
        }
        else {
            render_char(G->buffer[i][1], false, w);
        }
    }
}

/* render_tbuf: renders an entire text buffer */
void render_tbuf(tbuf B, WINDOW *w) {
    wmove(w, 0, 0);
    werase(w);

    dll L;
    for (L = B->start->next; L != B->end; L = L->next) {
        render_gapbuf(L->data, w);
    }

    // TODO add case for if the cursor is at the very bottom

    wrefresh(w);
}

/* renders a string within a WINDOW */
void render_string(WINDOW *w, char *str) {
    werase(w);
    int i, f;
    getbegx(w, i);
    getmaxx(w, f);
    // first clear the bar
    for (; i < f ; i++) {
        waddch(w, highlight(' '));
    }

    // next add the description
    wmove(w, 0, 1);
    wattron(WA_REVERSE);
    waddstr(w, str);
    wattroff(WA_REVERSE);
}

/* renders the header for the text editor */
void render_topbar(WINDOW *w) {
    render_string(w, "NetScribe, the terminal based cross-network editor -- ^X to exit")
}

void render_botbar(WINDOW *w) {
    werase(w);
    int i, f;
    getbegx(w, i);
    getmaxx(w, f);
    // first clear the bar
    for (; i < f ; i++) {
        waddch(w, highlight(' '));
    }
}

int establish_connection()

int main() {
    // build socket connection
    WINDOW *mainwin = initscr();
    // more stuff blah blah blah
}
