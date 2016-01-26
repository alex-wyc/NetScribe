#ifndef CENTRAL_SERVER_H_
#define CENTRAL_SERVER_H_

/* PREPROCESSOR DEFINITIONS ***************************************************/

#define MAX_CLIENT_PER_ROOM 10
#define MAX_SUBSERVER_COUNT 100 // maximum number of subservers (chat/co-editing rooms the server can handle)
#define MAX_CLIENT_COUNT 500 // maximum number of client connections

// for socket purposes
#define FROM_CLIENT 0
#define FROM_SUBSERV 1

#define CLIENT_PORT 11235
#define SUBSERV_PORT 11238

#define CONN_REQUEST "Connection Request: "

/* DATATYPE DEFINITIONS *******************************************************/

typedef struct client { // a client connection
    char name[16];
    int room_id; // which room this client is in. can we let it correspond to the index in rooms_list?
    int socket_id; // central server <-> client connection
} client;

typedef struct subserver {
    client *users[MAX_CLIENT_PER_ROOM];
} subserver;

typedef struct message {
    int client_id; // id of the client
    int to_distribute; // boolean, true if to distribute, false if internal
    char *cmd; // the command name and content
} message;

/* FUNCTION HEADERS ***********************************************************/
void debug(char *statement, ...);
void establish_connection(int *socket_c, int *socket_sub, int c_port, int s_port);
int listen_central(int *from_client, int sockets[]);
void check_error(int ret_val);
void handle_client(int socket, subserver *room_list[], client *users_list[]);
void handle_subserv(int socket, subserver *room_list[], client *users_list[]);

#endif // CENTRAL_SERVER_H_
