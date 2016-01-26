#include "central.h"

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

client *handshake_join_server(int fd, int index, char *buf); // handshake procedure when the client joins
subserver *create_new_room(int fd, int index, int room_no); // creates a new room and fills it 
void join_room(int id, subserver *room);

#endif // _PROTOCOL_H
