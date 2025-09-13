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

int main() {
    // Servidor del proceso 3
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un server_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_p3"
    };
    safe_bind(server_socket, &server_addr, sizeof(server_addr));    
    
    // Cliente del servidor 1
    struct sockaddr_un p1_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_p1"
    };
    int p1_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    safe_connect(p1_socket, &p1_addr, sizeof(p1_addr));
    
    int client_socket = listen_and_accept(server_socket, &server_addr);

    read_and_send(client_socket, p1_socket);
    
    close(server_socket);

    return 0;
}