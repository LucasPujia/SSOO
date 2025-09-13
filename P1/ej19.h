#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>

#define sockaddr_t struct sockaddr *
#define N 10

void safe_bind(int sockfd, struct sockaddr_un* addr, socklen_t addrlen) {
    unlink(addr->sun_path);

    if (bind(sockfd, (sockaddr_t) addr, addrlen) == -1) {
        perror("Error bindig");
        printf("Path: %s\n", addr->sun_path);
        exit(EXIT_FAILURE);
    };
}

void safe_listen2(int sockfd, struct sockaddr_un* addr, int n) {
    printf("Escuchando desde %s\n", addr->sun_path);
    
    if (listen(sockfd, n) == -1) {
        perror("Error listening");
        close(sockfd);
        exit(EXIT_FAILURE);
    };
}

void safe_listen(int sockfd, struct sockaddr_un* addr) {
    safe_listen2(sockfd, addr, 1);
}

int listen_and_accept(int sockfd, struct sockaddr_un* addr) {
    safe_listen(sockfd, addr);
    
    int client_addr;
    socklen_t len = sizeof(client_addr);
    int client_socket = accept(sockfd, (sockaddr_t) &client_addr, &len);
    printf("Conexión aceptada!\n");
    return client_socket;
}

void safe_connect(int sockfd, struct sockaddr_un* addr, socklen_t addrlen) {
    bool is_connected = 0;
    while(!is_connected) {
        printf("Conectando con %s...\n", addr->sun_path);
        if (connect(sockfd, (sockaddr_t) addr, addrlen) == -1) {
            perror("Error connecting");
            sleep(2);
        } else {
            is_connected = true;
        };
    }
    printf("Conexión exitosa!\n");
}

void read_and_send(int client_socket, int server_socket) {
    int numero = 0;

    while(numero < N) {
        int read_res = read(client_socket, &numero, sizeof(numero));
        if (read_res == -1) {
            perror("Error reading");
            exit(EXIT_FAILURE);
        } else if (read_res == 0) {
            printf("Mi trabajo aquí ha terminado. Llegué hasta %i\n", numero);
            break;
        }
        printf("Recibí %i, enviando %i\n", numero, numero+1);
        numero++;
        if (numero < N) {
            if (write(server_socket, &numero, sizeof(numero)) == -1) {
                perror("Error writing");
                exit(EXIT_FAILURE);
            }
        } else printf("Terminado con %i\n", numero);
    }
}
