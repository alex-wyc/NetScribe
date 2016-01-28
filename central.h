#ifndef CENTRAL_SERVER_H_
#define CENTRAL_SERVER_H_

/* PREPROCESSOR DEFINITIONS ***************************************************/

#define MAX_CLIENT_PER_ROOM 8
#define MAX_SUBSERVER_COUNT 100 // maximum number of subservers (chat/co-editing rooms the server can handle)
#define MAX_CLIENT_COUNT 500 // maximum number of client connections

#define CLIENT_PORT 9002

#define CONN_REQUEST "connreq"
#define SERVER_EXIT "exitserv"
#define BUF_REQUEST "bufferreq"

/* DATATYPE DEFINITIONS *******************************************************/

typedef struct client { // a client connection
    char name[16];
    int room; // which room this client is in
    int room_id; // id in the local/client side room
    int socket_id; // central server <-> client connection
} client;

typedef struct subserver { // wrapper struct subserver, a list of pointers to client connections
    int user_ids[MAX_CLIENT_PER_ROOM];
} subserver;

typedef struct message {
    int remote_client_id; // id of the client on the central server
    int local_client_id; // id of the client on the local server
    int to_distribute; // boolean, true if to distribute, false if internal
    char cmd[16]; // the command
    char content[256]; // the content of the command
} message;

/* FUNCTION HEADERS ***********************************************************/
void debug(char *statement, ...);
void check_error(int ret_val);
void handle_cmd_line_args(int argc, char *argv[]); // handles command line arguments
int establish_connection(); // sets up the socket file
void handle_client(int socket); // handles client arguments

#endif // CENTRAL_SERVER_H_
