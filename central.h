#ifndef CENTRAL_SERVER_H_
#define CENTRAL_SERVER_H_

//room and subserver are used interchangeably for now since idk if we're gonna change the struct name

typedef struct client { // a client connection
    char name[16];
    int room_id; // which room this client is in. can we let it correspond to the index in rooms_list?
    int socket_id; // central server <-> client connection
} client;

typedef struct subserver {
    //int pipe[2];
    client *users;
    int *on_line; // one-to-one correspondence with the users, if the user is online
} subserver;

void debug(char *statement, ...);
void establish_connection(int *socket_c, int *socket_sub, int c_port, int s_port);
int listen_central(int *from_client, int sockets[]);
void check_error(int ret_val);
void handle_client(int socket, subserver *room_list, client *users_list);
void handle_subserv(int socket, subserver *room_list, client *users_list);

#endif // CENTRAL_SERVER_H_
