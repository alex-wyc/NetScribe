/*******************************************************************************
 * Central Server Program for NetScribe                                        *
 *                                                                             *
 * Authors                                                                     *
 *  Yicheng Wang                                                               *
 *                                                                             *
 * Description                                                                 *
 *  Handles Public Channel Communications and requests from each subserver,    *
 *  also in charge of creating subservers and providing them with global       *
 *  informations.
 *
 *  Thorough *
 *                                                                             *
 ******************************************************************************/

/* TODO
 *  TODO-List
 */

/* Dev Log
 *  Project Created: 2016-01-15 13:45 - Yicheng W.
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

#include "central_server.h"

/* CONSTANTS ******************************************************************/

#define MAX_SUBSERVER_COUNT 100 // maximum number of subservers (chat/co-editing rooms the server can handle)

// for pipping purposes
#define FROM_CLIENT 0
#define FROM_SUBSERV 1

#define CLIENT_PORT 11235
#define SUBSERV_PORT 11238

int DEBUG; // print statements

/* MAIN ***********************************************************************/

int main(int argc, char *argv[]) {
    // vars
    
    int from_client, fd, i, ret_val;
    int sockets[2];
    char buf[256];

    // command line argument parser
    
    if (argc > 0) {
        for (i = 0 ; i < argc ; i++) {
            if (*argv[i] == '-') {
                switch (*(argv[i] + 1)) {
                    case 'd':
                        DEBUG = 1;
                        break;
                    // more cases to come
                }
            }
        }
    }

    // establishing socket

    establish_connection(&sockets[FROM_CLIENT], &sockets[FROM_SUBSERV], CLIENT_PORT, SUBSERV_PORT);

    // handle requests
    
    fd = listen_central(&from_client, sockets);

    ret_val = read(fd, buf, sizeof(buf));
    check_error(ret_val);

    if (from_client) { // handle client requests
        handle_client(buf);
    }
    else{
        handle_subserv(buf);
    }

    return 0;
}

/* UTIL FUNCTIONS *************************************************************/

/* debug: checks if debug is on, if so, print the statement, otherwise, do
 * nothing
 */
void debug (char *statement){
    if (DEBUG) {
        printf("%s\n", statement);
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
    struct sockaddr_in listener_c; // for incoming clients
    listener_c.sin_family = AF_INET;
    listener_c.sin_port = htons(c_port); // this is the port for incoming clients
    listener_c.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(*socket_c, (struct sockaddr*)&listener_c, sizeof(listener_c)) == -1) {
        fprintf(stderr, "Error %d at bind: %s", errno, strerror(errno));
        exit(1);
    }

    *socket_sub = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in listener_sub; // for subservers
    listener_sub.sin_family = AF_INET;
    listener_sub.sin_port = htons(s_port); // this is the port for subserv requests
    listener_sub.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(*socket_sub, (struct sockaddr*)&listener_sub, sizeof(listener_sub)) == -1) {
        fprintf(stderr, "Error %d at bind: %s\n", errno, strerror(errno));
        exit(1);
    } 
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

    int max_fd = (sockets[FROM_CLIENT], sockets[FROM_SUBSERV]) ? sockets[FROM_CLIENT] : sockets[FROM_SUBSERV]; // maxfd

    int status;

    if (select(max_fd + 1, &readfds, NULL, NULL, NULL) == -1) {
        fprintf(stderr, "Error %d at select: %s\n", errno, strerror(errno));
        exit(1);
    };

    if (FD_ISSET(sockets[FROM_CLIENT], &readfds)) { // data is avaliable on client side
        *from_client = 1;
        return accept(sockets[FROM_CLIENT], NULL, NULL);
    }

    else if (FD_ISSET(sockets[FROM_SUBSERV], &readfds)) { // data is avaliable from subserver
        *from_client = 0;
        return accept(sockets[FROM_SUBSERV], NULL, NULL);
    }  
}

/* handle_client: handles a client request
 * 
 * arguments:
 *     char *buf: the string that the client sent
 *
 * TODO what it actually does
 */
void handle_client (char *buf){
    // TODO
}

/* handle_subserv: handles a subserver request
 *
 * arguments:
 *     char *buf: the string that the subserver sent
 *
 * TODO what it actually does
 */
void handle_subserv (char *buf){
    // TODO
}
