#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "distribute.h"
#include "central.h"

int main() {
}

/* parse: parses an incoming string buf into a message struct
 *
 * buf string takes the form of:
 *     <client_id>`<d/s>`<cmd>`<content>
 */
message *parse (char *buf) {
    char *input;
    message *incoming = (message *)malloc(sizeof(message));

    input = strsep(&buf, "`"); // first occurance --> the id of the client
    incoming->remote_client_id = atoi(input);

    input = strsep(&buf, "`"); // second occurance --> to distribute or for subserver
    incoming->to_distribute = (*input == 'd');

    incoming->cmd = strsep(&buf, "`"); // third occurance is the command

    incoming->content = buf; // what is left is the content

    return incoming;
}

/* distribute: sends the incoming message across a list of user_ids
 * TODO long description
 */
void distribute (int user_ids[], int sz, message incoming) {
    int i;
    for (i = 0 ; i < sz ; i++) {
        if (user_ids[i] != -1) {
            write (users_list[i]->socket_id, &incoming, sizeof(incoming));
        }
    }
}

