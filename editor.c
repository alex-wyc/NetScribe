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
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gap_buffer.h"
#include "text_buffer.h"
#include "editor_backend.h"
#include "central.h"

/* server side definitions */
#define SERVER_ADDR "127.0.0.1"


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
    int i;
    // first clear the bar
    for (i = getbegx(w); i < getmaxx(w) ; i++) {
        waddch(w, highlight(' '));
    }

    // next add the description
    wmove(w, 0, 1);
    wattron(w, WA_REVERSE);
    waddstr(w, str);
    wattroff(w, WA_REVERSE);
}

/* renders the header for the text editor */
void render_topbar(WINDOW *w) {
    render_string(w, "NetScribe, the terminal based cross-network editor -- ^X to exit, ^L to refresh the page");
}

void render_botbar(WINDOW *w) {
    werase(w);
    int i; 
    // first clear the bar
    for (i = getbegx(w); i < getmaxx(w) ; i++) {
        waddch(w, highlight(' '));
    }
}

int main(int argc, char **argv) {
    message *to_send = (message *)malloc(sizeof(message)); // message struct to talk to server with
    message *distributed = (message *)malloc(sizeof(message));
    client *me = (client *)malloc(sizeof(client)); // ME!

    // build socket connection
    int from_server;
    if ((from_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Fatal - Cannot create socket\n");
        close(1);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(CLIENT_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (connect(from_server, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Fatal - Connection to server failed\n");
        close(1);
    }

    // setup terminal args and handshake with server
    // 1. join server request
    strncpy(to_send->cmd, CONN_REQUEST, sizeof(CONN_REQUEST));
    write(from_server, to_send, sizeof(message));
    read(from_server, me, sizeof(client));
    to_send->remote_client_id = me->socket_id;

    // let us assume that you are creating a room here

    // setup GUI
    WINDOW *mainwin = initscr();
    cbreak();
    noecho();
    keypad(mainwin, true);
    int vis = curs_set(0);

    int ncols = getmaxx(mainwin);
    int nlines = getmaxy(mainwin);
    int begx = getbegx(mainwin);
    int begy = getbegy(mainwin);

    WINDOW *canvas = subwin(mainwin,
                            nlines - 2, // save 2 lines for bottom status
                            ncols, // same as main
                            begy + 2, // save one line for title and one line for 'chat'
                            begx);

    WINDOW *topbar = subwin(mainwin, 1, ncols, begy, begx);
    WINDOW *chatbar = subwin(mainwin, 1, ncols, begy + 1, begx);
    WINDOW *botbar = subwin(mainwin, 1, ncols, nlines - 2, begx);

    render_topbar(topbar);
    render_botbar(botbar);

    // setup the select program
}
