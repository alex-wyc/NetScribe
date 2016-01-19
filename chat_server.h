typedef struct connection {
    char name[32]; // username of the client
    int pipe[2]; // the pipe used
} connection;
