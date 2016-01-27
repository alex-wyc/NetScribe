#include "central.h"

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

client *handshake_join_server(int fd, int index, message *incoming); // handshake procedure when the client joins
subserver *create_new_room(int fd, int index, int room_no); // creates a new room and fills it 
int join_room(int id, subserver *room);
void distribute(int user_ids[], int sz, client *users_list[], message incoming); // sends the message across a list of user ids with size sz


#endif // _PROTOCOL_H
