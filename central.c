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
#define TO 0 
#define FROM 1

int DEBUG; // print statements

/* MAIN ***********************************************************************/

int main(int argc, char *argv[]) {
    int i;

    // command line argument parser
    
    if (argc > 0) {
        for (int i = 0 ; i < argc ; i++) {
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

    int socket_id_c, socket_client;

    socket_id_c = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in listener_c; // for incoming clients
    listener_c.sin_family = AF_INET;
    listener_c.sin_port = htons(11235); // this is the port for incoming clients
    listener_c.sin_addr.s_addr = INADDR_ANY;
    
    bind(socket_id_c, (struct sockaddr*)&listener_c, sizeof(listener_c));

    struct sockaddr_in listener_sub; // for subservers
    listener_sub.sin_family = AF_INET;
    listener_sub.sin_port = htons(11238); // this is the port for subserv requests
    listener_sub.sin_addr.s_addr = inet_addr("127.0.0.1");
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
