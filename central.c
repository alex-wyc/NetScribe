/*******************************************************************************
 * Central Server Program for NetScribe                                        *
 *                                                                             *
 * Authors                                                                     *
 *  Yicheng Wang                                                               *
 *                                                                             *
 * Description                                                                 *
 *  Handles Public Channel Communications and requests from each subserver,    *
 *  also in charge of creating subservers and providing them with global       *
 *  informations.                                                              *
 *                                                                             *
 *  Handles:                                                                   *
 *      - Room (subserver) creation                                            *
 *      - User invitation                                                      *
 *      - Requests for full user list                                          *
 *                                                                             *
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

#include "central_server.h"

/* CONSTANTS ******************************************************************/

#define MAX_SUBSERVER_COUNT 100 // maximum number of subservers (chat/co-editing rooms the server can handle)
#define MAX_CLIENT_COUNT 500 // maximum number of client connections

// for socket purposes
#define FROM_CLIENT 0
#define FROM_SUBSERV 1

#define CLIENT_PORT 11235
#define SUBSERV_PORT 11238

#define CONN_REQUEST "Connection Request"

int DEBUG = 0; // print statements, off by default

char *help = ""; // help doc string TODO

///client NO_CLIENT = (client *)malloc(sizeof(client));
///memset(NO_CLIENT, 0, sizeof(client));


/* MACROS *********************************************************************/
#define REMOVE_ELEMENT(list, index, sz) memset(list + index, 0, sz)

/* MAIN ***********************************************************************/

int main(int argc, char *argv[]) {

    // vars
    
    int from_client, fd, i, ret_val;
    int sockets[2];
    char buf[256];

    subserver *rooms_list = (subserver *)calloc(MAX_SUBSERVER_COUNT, sizeof(subserver));

    client *users_list = (client *)calloc(MAX_CLIENT_COUNT, sizeof(client));
 
    // command line argument parser
    
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

    // establishing socket

    establish_connection(&sockets[FROM_CLIENT], &sockets[FROM_SUBSERV], CLIENT_PORT, SUBSERV_PORT);

    // handle requests

    while (1) {
        fd = listen_central(&from_client, sockets);

        if (from_client) { // handle client requests
            handle_client(fd, rooms_list, users_list);
        }
        else{
            handle_subserv(fd, rooms_list, users_list);
        }
    }

    return 0;
}

/* UTIL FUNCTIONS *************************************************************/

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

/* establish_connection: creates and binds the sockets to both client and
 * subservers to their respective ports
 *
 * arguments:
 *     int *socket_c: to-be-filled socket that listens to clients
 *     int *socket_sub: to-be-filled socket that listens to subservers
 *     int c_port: the port to listen on for client connections
 *     int s_port: the port to listen on for subserver connections
 *
 * returns:
 *     modifies socket_c and socket_sub
 */
void establish_connection (int *socket_c, int *socket_sub, int c_port, int s_port){
    *socket_c = socket(AF_INET, SOCK_STREAM, 0);

    debug("<central_server>: socket to client created\n");

    struct sockaddr_in listener_c; // for incoming clients
    listener_c.sin_family = AF_INET;
    listener_c.sin_port = htons(c_port); // this is the port for incoming clients
    listener_c.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(*socket_c, (struct sockaddr*)&listener_c, sizeof(listener_c)) == -1) {
        fprintf(stderr, "Error %d at bind: %s", errno, strerror(errno));
        exit(1);
    }

    debug("<central_server>: socket to client binded to address INADDR_ANY at port %d\n", CLIENT_PORT);

    *socket_sub = socket(AF_INET, SOCK_STREAM, 0);

    debug("<central_server>: socket to subservers created\n");

    struct sockaddr_in listener_sub; // for subservers
    listener_sub.sin_family = AF_INET;
    listener_sub.sin_port = htons(s_port); // this is the port for subserv requests
    listener_sub.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(*socket_sub, (struct sockaddr*)&listener_sub, sizeof(listener_sub)) == -1) {
        fprintf(stderr, "Error %d at bind: %s\n", errno, strerror(errno));
        exit(1);
    } 

    debug("<central_server>: socket to subservers binded to address 127.0.0.1 at port %d\n", SUBSERV_PORT);
}

/* listen_central: blocks until the read queue of either the server connection
 * or client connection is open, then returns 
 * 
 * arguments:
 *     int *from_client: boolean value as to if the connection is from client
 *     int sockets[]: the sockets to listen to
 * 
 * returns:
 *     a file descriptor to the accepted connection, also modifies from_client
 */
int listen_central (int *from_client, int sockets[]){
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(sockets[FROM_CLIENT], &readfds);
    FD_SET(sockets[FROM_SUBSERV], &readfds);

    debug("<central_server>: awaiting connection...\n");

    if (select(2, &readfds, NULL, NULL, NULL) == -1) {
        fprintf(stderr, "Error %d at select: %s\n", errno, strerror(errno));
        exit(1);
    }

    if (FD_ISSET(sockets[FROM_CLIENT], &readfds)) { // data is avaliable on client side
        *from_client = 1;
        debug("<central_server>: client connects\n");
        return accept(sockets[FROM_CLIENT], NULL, NULL);
    }

    else if (FD_ISSET(sockets[FROM_SUBSERV], &readfds)) { // data is avaliable from subserver
        *from_client = 0;
        debug("<central_server>: subserver connects\n");
        return accept(sockets[FROM_SUBSERV], NULL, NULL);
    }  
}

/* handle_client: handles a client request
 * 
 * arguments:
 *     socket: the fd of the incoming socket connection
 *     rooms_list: the array of subservers
 *     users_list: the array of clients
 *
 * TODO what it actually does
 */
void handle_client (int socket, subserver *rooms_list, client *users_list){
    char buf[256];
    int ret_val, c;

    ret_val = read(socket, buf, sizeof(buf));
    check_error(ret_val);

    if (strncmp(buf, CONN_REQUEST, sizeof(CONN_REQUEST)) == 0) { // if this is a conn request
        client *new_connection;
        for (c = 0 ; c < MAX_CLIENT_COUNT ; c++) {
            if (*(users_list + c) == 0) {
                new_connection = users_list + c;
                break;
            }
        }

        
    }
}

/* handle_subserv: handles a subserver request
 *
 * arguments:
 *     char *buf: the string that the subserver sent
 *
 * TODO what it actually does
 */
void handle_subserv (int socket, subserver *rooms_list, client *users_list){
    // TODO
}
