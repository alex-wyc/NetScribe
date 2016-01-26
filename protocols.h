#include "central.h"

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

client *handshake_join_server(int fd, int index, char *buf); // handshake procedure when the client joins

#endif // _PROTOCOL_H
