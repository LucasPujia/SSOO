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
    // Servidor del proceso 1
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un server_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_p1"
    };
    safe_bind(server_socket, &server_addr, sizeof(server_addr));    

    // Acepto la conexión del cliente
    int client_socket = listen_and_accept(server_socket, &server_addr);

    // Cliente del proceso 2
    int p2_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un p2_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_p2"
    };

    safe_connect(p2_socket, &p2_addr, sizeof(p2_addr));

    // Recibo y envío mensajes
    int numero = 0;
    printf("Iniciando cadena con %i\n", numero);
    write(p2_socket, &numero, sizeof(numero));
    read_and_send(client_socket, p2_socket);

    close(server_socket);

    return 0;
}