#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "chat_room.h"
#include "central.h"

int main() {
}
/* run_chat_room can be called directly in central.c
 * arguments: 
 *     int socket_sender: central <-> client connection socket of the client 
 * that sent the message
 *     
 */
int run_chat_room(char * message, int socket_sender,  client * users_list, subserver * rooms_list) {
    client * sender;
    int i = 0;
    while (users_list[i].name) {
	if (users_list[i].socket_id == socket_sender) {
	    sender = &users_list[i];
	    return;
	}
	i++;
    }
    if (users_list[i].name == NULL) {
	printf("ERROR: Socket not found in users_list\n");
	return -1;
    }
    distribute_message(message, sender, &rooms_list[sender->room_id]);
}

/* arguments: sender and room are indexed to the relevant single client/
 * subserver in users_list and rooms_list
 */
int distribute_message(char * message, client * sender, subserver * room) {
    char * formatted = format_message(sender->name, message);
    int ret_val;
    int i = 0;
    while (room->users[i].name) {
	ret_val = write(room->users[i].socket_id, formatted, strlen(formatted));
	check_error(ret_val);
	i++;
    }
    return 0; 
}

char * format_message(char * name, char * message) {
    char *formatted = (char *)calloc(2048, sizeof(char));
    strcat(formatted, name);
    strcat(formatted, ":: ");
    strncat(formatted, message, 2048 - strlen(message) - strlen(name) - 5);
    strcat(formatted, "\n"); 
    return formatted;
}
