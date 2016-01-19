void debug(char *statement);
void establish_connection(int *socket_c, int *socket_sub, int c_port, int s_port);
int listen_central(int *from_client, int sockets[]);
void check_error(int ret_val);
void handle_client(char *buf);
void handle_subserv(char *buf);
