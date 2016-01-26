/*******************************************************************************
 * this is the file for the different protocols involved in the making of the  *
 * server file                                                                 *
 *                                                                             *
 * Authors                                                                     *
 *  Yicheng Wang <alex-wyc>                                                    *
 *                                                                             *
 * Description                                                                 *
 *  List of Server Side Protocols Used, these include:                         *
 *   - handshake_join_server: for joining server                               *
 *   - create_room: for creating a room                                        *
 *   - handshake_join_room: for joining room                                   *
 *                                                                             *
 *  More information enclosed within comments for each protocol                *
 *                                                                             *
 *******************************************************************************/

/* TODO
 *  TODO-List
 */

/* Dev Log
 *  Project Created: 2016-01-25 22:50 - Yicheng W.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "protocols.h"
#include "central.h"

/* *handshake_join_server: the handshake procedure from the server side when the
 * client joins, writes to the file descriptor the client class basically
 * TODO long description
 */
client *handshake_join_server (int fd, int index, char *buf) {
    client *new_connection = (client *)malloc(sizeof(client));
    new_connection->socket_id = fd;

    strncpy(new_connection->name,
            buf + sizeof(CONN_REQUEST),
            sizeof(new_connection->name));

    write(fd, new_connection, sizeof(client));
    return new_connection;
}

/* create_new_room: create a new chat room, and relays information to the client
 *
 * Sends to client: the index of the room, 1 int
 */
subserver *create_new_room (int fd, int index, int room_no){
    int c;
    subserver *new_room = (subserver *)malloc(sizeof(subserver));

    for (c = 0 ; c < MAX_CLIENT_PER_ROOM ; c++) {
        new_room->user_ids[c] = -1; // default, see distribute
    }

    new_room->user_ids[0] = index;

    write(fd, &room_no, sizeof(int));

    return new_room;
}

void join_room(int id, subserver *room) {
    int c;
    for (c = 0 ; c < MAX_CLIENT_PER_ROOM ; c++) {
        if (room->user_ids[c] == -1) {
            room->user_ids[c] = id;
            return;
        }
    }
}
