#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include "ej19.h"

int main() {
    int server_socket;
    struct sockaddr_un server_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_wpp"
    };

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect(server_socket, (sockaddr_t) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    int i;
    if (read(server_socket, &i, sizeof(i)) == -1) {
        perror("La conexión ha sido cerrada");
        exit(EXIT_FAILURE);
    }

    // if (fork() == 0) {
    //     while (true) {
    //         int mensaje;
    //         if(read(server_socket, &mensaje, sizeof(mensaje)) == -1) {
    //             perror("Error reading");
    //         };  
    //         printf("%i | Mensaje recibido! Es %i", i, mensaje);
    //     }
    // }

    while(true) {
        int numero = 0;
        printf("%i | Introduce un número: ", i);
        if (scanf("%i", &numero) != 1) {
            printf("%i | Entrada no válida.\n", i);
            int c; // limpiar stdin
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (numero == -69) {
            printf("Mi trabajo aquí ha terminado\n");
            break;
        }

        if (write(server_socket, &numero, sizeof(numero)) == -1) {
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }

    close(server_socket);
    exit(EXIT_SUCCESS);
}
