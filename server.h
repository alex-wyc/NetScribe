#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

typedef struct connection {
    int id; // id of the connection, also the index of it along the pipes
    char name[32]; // username of the client
} connection

#endif
