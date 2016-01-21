#ifndef CENTRAL_SERVER_H_
#define CENTRAL_SERVER_H_

typedef struct client { // a client connection
    char name[16];
    int sub_serv_id; // which subserver this client is connected to
} client;

typedef struct subserver { // a subserver connection
    int pipe[2];
    client *users;
    int *on_line; // one-to-one correspondence with the users, if the user is online
} subserver;

void debug(char *statement, ...);
void establish_connection(int *socket_c, int *socket_sub, int c_port, int s_port);
int listen_central(int *from_client, int sockets[]);
void check_error(int ret_val);
void handle_client(char *buf, subserver *room_list, client *user_list);
void handle_subserv(char *buf, subserver *room_list, client *user_list);

#endif // CENTRAL_SERVER_H_
