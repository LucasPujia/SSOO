#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include "ej19.h"

#define sockaddr_t struct sockaddr *

int main() {
    // Servidor del proceso 2
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un server_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_p2"
    };
    safe_bind(server_socket, &server_addr, sizeof(server_addr));    

    
    // Cliente del servidor 3
    struct sockaddr_un p3_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_p3"
    };
    int p3_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    safe_connect(p3_socket, &p3_addr, sizeof(p3_addr));

    int client_socket = listen_and_accept(server_socket, &server_addr);

    // Recibo y envío mensajes
    read_and_send(client_socket, p3_socket);

    close(server_socket);

    return 0;
}