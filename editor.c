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
#include <errno.h>
#include <time.h>

#include "gap_buffer.h"
#include "text_buffer.h"
#include "editor_backend.h"
#include "central.h"

/* server side definitions */
#define SERVER_ADDR "127.0.0.1"

#define EDIT_MODE 0
#define CHAT_MODE 1
#define SAVE_MODE 2

#define highlight(x) x | A_STANDOUT


int USER_ME; // this is the local index of me
char NAME[16];
char *HELP = ""; //help doc
int CONN = 1; // if we access the network
int ROOM_NO = 0; // room number of the client
int FROM_SERVER = -1;
int TO_SERVER = -1;
int mode = EDIT_MODE; // by default
char usernames[MAX_CLIENT_PER_ROOM][16] = {0}; // names of users

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
    printf("function is called\n");
    wmove(w, 0, 0);
    werase(w);
    printf("initial setup done\n");

    dll L;
    if (!B->start->next) {
        printf("yeah that's null\n");
    }
    printf("%p\n", B->start->next);
    for (L = B->start->next; L != B->end; L = L->next) {
        printf("gap buf rendered\n");
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
    render_string(w, "NetScribe, the terminal based cross-network editor");
}

void render_botbar(WINDOW *w) {
    render_string(w, "^X Exit \t ^L Clear \t ^O Write \t ^H Chat");
}

int main(int argc, char **argv) {
    message *to_send = (message *)malloc(sizeof(message)); // message struct to talk to server with
    message *received = (message *)malloc(sizeof(message));
    client *me = (client *)malloc(sizeof(client)); // ME!
    struct sockaddr_in serv_addr;
    tbuf B = 0;

    // handle command line args
    int i;
    if (argc > 0) {
        for (i = 0 ; i < argc ; i++) {
            if (*argv[i] == '-') {
                switch(*(argv[i] + 1)) {
                    case 'n':
                        strncpy(NAME, argv[++i], sizeof(NAME));
                        break;

                    case 'h':
                        printf("%s\n", HELP);
                        break;

                    case 'l': // local use
                        CONN = 0;
                        break;

                    case 'j': // not creating a room, but joining
                        ROOM_NO = atoi(argv[++i]);
                        break;
                }
            }
            else {
                B = read_from_file(argv[i]);
                printf("reading done\n");
            }
        }
    }

    if (!B) {
        B = new_tbuf();
    }

    // build socket connection
    if (CONN) {
        if ((FROM_SERVER = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr, "Fatal - Cannot create socket\n");
            close(1);
        }

        if ((TO_SERVER = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr, "Fatal - Cannot create socket\n");
            close(1);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(CLIENT_PORT);
        serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

        if (connect(FROM_SERVER, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            fprintf(stderr, "Fatal - Connection to server failed\n");
            close(1);
        }

        // setup terminal args and handshake with server
        // 1. join server request
        strncpy(to_send->cmd, CONN_REQUEST, sizeof(CONN_REQUEST));
        strncpy(to_send->content, NAME, sizeof(NAME));
        printf("Name sent to server: %s\n", to_send->content);
        write(FROM_SERVER, to_send, sizeof(message));
        read(FROM_SERVER, &to_send->remote_client_id, sizeof(int));
        printf("Received from server: %d\n", to_send->remote_client_id);

        // let us assume that you are creating a room here
        
        if (ROOM_NO) { // join a given room
            // TODO
        }
        else { // create a room
            strncpy(to_send->cmd, "new", 4);
            printf("command sent: %s\n", to_send->cmd);
            if (connect(TO_SERVER, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                fprintf(stderr, "Fatal - Connection to server failed when creating room\n");
                close(1);
            }
            write(TO_SERVER, to_send, sizeof(message));
            read(FROM_SERVER, &ROOM_NO, sizeof(int));
            to_send->local_client_id = 0; // create a room = 0
            me->room = ROOM_NO;
            me->room_id = 0;
            strncpy(usernames[me->room_id], NAME, sizeof(NAME));
            printf("recv from server: in room %d\n", me->room);
        }
    }

    //getchar();

    // setup GUI
    WINDOW *mainwin = initscr();
    cbreak();
    //noecho();
    keypad(mainwin, true);
    int vis = curs_set(0);

    int ncols = getmaxx(mainwin);
    int nlines = getmaxy(mainwin);
    int begx = getbegx(mainwin);
    int begy = getbegy(mainwin);

    printf("%d, %d, %d, %d\n", ncols, nlines, begx, begy);

    WINDOW *canvas = subwin(mainwin,
                            nlines - 2, // save 2 lines for bottom status
                            ncols, // same as main
                            begy + 2, // save one line for title and one line for 'chat'
                            begx);

    WINDOW *topbar = subwin(mainwin, 1, ncols, begy, begx);
    WINDOW *chatbar = subwin(mainwin, 1, ncols, begy + 1, begx);
    WINDOW *botbar = subwin(mainwin, 1, ncols, nlines - 2, begx);
    WINDOW *inputbar = subwin(mainwin, 1, ncols, nlines - 1, begx);

    render_topbar(topbar);
    render_string(chatbar, "");
    render_botbar(botbar);

    printf("setup done\n");

    // setup the select program
    char c[3];
    
    while (1) {
        render_tbuf(B, canvas);
        fd_set readfds;

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(FROM_SERVER, &readfds);

        if (select(2, &readfds, NULL, NULL, NULL) < 0) {
            fprintf(stderr, "Error %d: %s", errno, strerror(errno));
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) { // reading from stdin
            read(STDIN_FILENO, &c, 1);

            if (c[0] == 12) { // ^L --> redraw
                wclear(mainwin);
                render_topbar(topbar);
                render_string(chatbar, "");
                render_tbuf(B, canvas);
                wrefresh(mainwin);
            }

            else { // these will be sent
                strncpy(to_send->cmd, "edit", 5);

                if (c[0] == 27) {
                    read(STDIN_FILENO, &c[1], 2);
                    switch (c[2]) {
                        case 'C': // move cursor right
                            backward_char(B, me->room_id);
                            break;

                        case 'D':
                            forward_char(B, me->room_id);
                            break;
                    }
                    memcpy(to_send->content, c, 3);
                }

                else if (c[0] == 24) { // ^X --> exit
                    curs_set(vis);
                    endwin();
                    if (CONN) { // send off closing request
                        strncpy(to_send->cmd, "exit", 5);
                    }
                    printf("Exiting...\n");
                    return 0;
                }

                else if (c[0] == 127) { // backspace
                    delete_char(B, me->room_id);
                    memcpy(to_send->content, c, 3);
                }

                else if (0 < c[0] && c[0] < 127) { // other characters
                    insert_char(B, c[0], me->room_id);
                    memcpy(to_send->content, c, 3);
                }

                if (connect(TO_SERVER, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                    fprintf(stderr, "Fatal - Connection to server failed when creating room\n");
                    close(1);
                }

                write(TO_SERVER, to_send, sizeof(message));
            }
        }

        else if (FD_ISSET(FROM_SERVER, &readfds)) {
            read(FROM_SERVER, received, sizeof(message));
            int c_user = received->local_client_id;

            if (strstr(received->cmd, "chat")) { // this is a chat command
                time_t t;
                struct tm *tinfo;
                time(&t);
                tinfo = localtime(&t);
                char time_s[20];
                if (!strftime(time_s, sizeof(time_s), "%H:%M:%S", tinfo)) {
                    fprintf(stderr, "Error %d: %s", errno, strerror(errno));
                    exit(1);
                }
                char to_put_up[100];
                sprintf(to_put_up, "[%s] %s: %s", time_s,
                        usernames[received->local_client_id],
                        received->content);

                render_string(chatbar, to_put_up);
            }

            if (strstr(received->cmd, "join")) { // if new user joins
                strncpy(usernames[received->local_client_id], received->content, 16);
                time_t t;
                struct tm *tinfo;
                time(&t);
                tinfo = localtime(&t);
                char time_s[20];
                if (!strftime(time_s, sizeof(time_s), "%H:%M:%S", tinfo)) {
                    fprintf(stderr, "Error %d: %s", errno, strerror(errno));
                    exit(1);
                }
                char to_put_up[100];
                sprintf(to_put_up, "[%s] <SERVER>: %s has joined", time_s,
                        usernames[received->local_client_id]);

                render_string(chatbar, to_put_up);
                // TODO -- set cursor
            }

            if (strstr(received->cmd, "exit")) { // if user exits
                time_t t;
                struct tm *tinfo;
                time(&t);
                tinfo = localtime(&t);
                char time_s[20];
                if (!strftime(time_s, sizeof(time_s), "%H:%M:%S", tinfo)) {
                    fprintf(stderr, "Error %d: %s", errno, strerror(errno));
                    exit(1);
                }
                char to_put_up[100];
                sprintf(to_put_up, "[%s] <SERVER>: %s has exited", time_s,
                        usernames[received->local_client_id]);

                render_string(chatbar, to_put_up);
                memset(usernames[received->local_client_id], 0, sizeof(usernames[received->local_client_id]));
                // TODO -- remove cursor
            }

            if (strstr(received->cmd, "edit")) {
                memcpy(c, received->content, sizeof(c));

                if (c[0] == 27) {
                    switch (c[2]) {
                        case 'C': // move cursor right
                            backward_char(B, c_user);
                            break;

                        case 'D':
                            forward_char(B, c_user);
                            break;
                    }
                }

                else if (c[0] == 127) { // backspace
                    delete_char(B, c_user);
                }

                else if (0 < c[0] && c[0] < 127) { // other characters
                    insert_char(B, c[0], c_user);
                }
            }
        }
    }

    if (CONN) { // send off closing request
        // TODO send closing request to server
    }

    curs_set(vis);
    endwin();
    printf("Exiting...\n");
    return 0;
}
