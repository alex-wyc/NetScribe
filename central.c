/*******************************************************************************
 * Central Server Program for NetScribe                                        *
 *                                                                             *
 * Authors                                                                     *
 *  Yicheng Wang <alex-wyc>                                                    *
 *                                                                             *
 * Description                                                                 *
 *  Handles Public Channel Communications and requests from each subserver,    *
 *  also in charge of creating subservers and providing them with global       *
 *  informations.                                                              *
 *                                                                             *
 * Theory:                                                                     *
 *  This is the main executable file for the central server.                   *
 *                                                                             *
 *  It calls functions from distribute.c, protocol.c and util.c                *
 *                                                                             *
 *  Basic idea is that we have an aray of pointers to client struct that is    *
 *  the list of all the users and another array of pointers to the subserver   *
 *  wrapper class that contains a list of client_ids (indices in the client    *
 *  struct). These two arrays are open-to-edit to all functions.               *
 *                                                                             *
 *  The main function creates a socket to listen on any incoming addresses,    *
 *  once a connection is avaliabe, it is sent to RO fd on the server side, the *
 *  client writes the command, and both sides closes the descriptor. This rule *
 *  is broken during setup of the client.                                      *
 *                                                                             *
 *  However, persistent WO sockets (for the server side to the client side)    *
 *  exist within each user struct, which is what the server uses to talk back  *
 *  to the client.                                                             *
 *******************************************************************************/

/* TODO
 *  Handle client and subserver request
 *  Have global variables to do various tasks
 */

/* Dev Log
 *  Project Created: 2016-01-15 13:45 - Yicheng W.
 *  Basic Connection establishment completed: 2016-01-19 15:35 - Yicheng W.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdarg.h>

#include "central.h"
#include "distribute.h"
#include "protocols.h"

/* CONSTANTS ******************************************************************/

int DEBUG = 0; // print statements, off by default

const char *help = ""; // help doc string TODO

/* GLOBAL VARIABLES ***********************************************************/
client *users_list[MAX_CLIENT_COUNT] = {0};
subserver *rooms_list[MAX_SUBSERVER_COUNT] = {0};

/* MAIN ***********************************************************************/

int main(int argc, char *argv[]) {

    // vars
    int from_client, fd, ret_val;

    // command line argument parser
    handle_cmd_line_args(argc, argv);

    // establishing socket
    from_client = establish_connection();

    ret_val = listen(from_client, MAX_CLIENT_COUNT);
    check_error(ret_val);

    // handle requests
    while (1) {
        fd = accept(from_client, NULL, NULL);

        handle_client(fd);

        close(fd); // closes the file descriptor after the thing is done, note that this fd is READ ONLY!
    }

    return 0;
}

/* UTIL FUNCTIONS *************************************************************/

/* handle_client: handles a client request
 * 
 * arguments:
 *     socket: the fd of the incoming socket connection
 *
 * TODO what it actually does
 */
void handle_client (int socket){
    char buf[256];
    int ret_val, c;

    ret_val = read(socket, buf, sizeof(buf));
    check_error(ret_val);

    if (strncmp(buf, CONN_REQUEST, sizeof(CONN_REQUEST)) == 0) { // if this is a conn request
        for (c = 0 ; c < MAX_CLIENT_COUNT ; c++) {
            if (users_list[c] == 0) {
                users_list[c] = handshake_join_server(socket, c, buf);
            }
        }
    }

    else { // this is not a handshake request, parse input string and execute
        message *incoming = parse(buf);

        if (incoming->to_distribute) { // if this is a command to distribute
            subserver *local = rooms_list[users_list[incoming->remote_client_id]->room];
            distribute(local->user_ids, MAX_CLIENT_PER_ROOM, *incoming);
        }

        else {
            if (strstr(incoming->cmd, "new")) {
                for (c = 0 ; c < MAX_SUBSERVER_COUNT ; c++) {
                    if (rooms_list[c] == 0) {
                        client *sender = users_list[incoming->remote_client_id];
                        rooms_list[c] = create_new_room(sender->socket_id, incoming->remote_client_id, c);
                    }
                }
            }

            if (strstr(incoming->cmd, "join")) {
                join_room(incoming->remote_client_id, rooms_list[atoi(incoming->content)]);
            }
        }
    }
}

/* handle_cmd_line_args: handles command line options and arguments
 */
void handle_cmd_line_args (int argc, char *argv[]){
    int i;
    if (argc > 0) {
        for (i = 0 ; i < argc ; i++) {
            if (*argv[i] == '-') {
                switch (*(argv[i] + 1)) {
                    case 'd':
                        DEBUG = 1;
                        break;

                    case 'h':
                        printf("%s", help);
                        break;

                    default:
                        printf("Unknown option: %s\nFor help run ./central_server.out -h\nexiting...\n", argv[i]);
                        exit(1);
                        // more cases to come
                }
            }
        }
    }
}

/* establish_connection: creates and binds the sockets to client connection
 *
 * generates a socket that listens to clients, it will bind to INADDR_ANY and
 * listen on port CLIENT_PORT as defined in the header
 *
 * returns:
 *     the file descriptor of the resultant socket
 */
int establish_connection (){
    int socket_c = socket(AF_INET, SOCK_STREAM, 0);

    debug("<central_server>: socket to client created\n");

    struct sockaddr_in listener_c; // for incoming clients
    listener_c.sin_family = AF_INET;
    listener_c.sin_port = htons(CLIENT_PORT); // this is the port for incoming clients
    listener_c.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_c, (struct sockaddr*)&listener_c, sizeof(listener_c)) == -1) {
        fprintf(stderr, "Error %d at bind: %s", errno, strerror(errno));
        exit(1);
    }

    debug("<central_server>: socket to client binded to address INADDR_ANY at port %d\n", CLIENT_PORT);

    return socket_c;
}

/* debug: checks if debug is on, if so, print the statement, otherwise, do
 * nothing
 */
void debug (char *format, ...){
    if (DEBUG) {
        va_list strings;
        int done;
        va_start(strings, format);
        done = vfprintf(stdout, format, strings);
        va_end(strings);
    }
}

/* check_error: checks for error in return value, exits the program if there is
 * error
 */
void check_error (int ret_val){
    if (ret_val == -1) {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}
