#include "central.h"

int run_chat_room(char * message, int socket_sender, client * users_list, subserver * rooms_list);
int distribute_message(char * message, client * sender, subserver * room);
char * format_message(char * name, char * message);
message *parse(char *buf); // parses buffer into an incoming message struct
