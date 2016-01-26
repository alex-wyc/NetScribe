#include "central.h"

#ifndef _DISTRIBUTE_H
#define _DISTRIBUTE_H

message *parse(char *buf); // parses buffer into an incoming message struct
void distribute(int user_ids[], int sz, client *users_list[], message incoming); // sends the message across a list of user ids with size sz

#endif // _DISTRIBUTE_H
