#include "ej19.h"
#include <math.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>

enum { READ, WRITE };
#define MAX_CONNECTIONS 2

int connections_len = 0;
int connections[MAX_CONNECTIONS] = {0};
int pipes[MAX_CONNECTIONS][2];
int pipe_master[2];

void safe(int response_code) {
    if (response_code == -1) {
        perror("Safe error");
        exit(EXIT_FAILURE);
    }
    return response_code;
}

void connection_closed() {    
    int child_pid = wait(NULL);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i] == child_pid) connections[i] = 0;
    }
    connections_len--;
}

void message_received() {
    int mensaje;
    read(pipe_master[READ], &mensaje, sizeof(mensaje));
    printf("Transmitiendo el mensaje %i a los managers\n", mensaje);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i] != 0) {
            printf("%i\n", i);
            // safe(write(pipes[i][WRITE], &mensaje, sizeof(mensaje)));
        }
    }
}

void manage_client(int client_socket, int index){
    int reading_child_pid = fork();
    if (reading_child_pid == 0) {
        int broadcast;
        while(true) {
            safe(read(pipes[index][READ], &broadcast, sizeof(broadcast)));
            printf("Soy manager, leí %i\n", broadcast);
            safe(write(client_socket, &broadcast, sizeof(broadcast)));
            printf("Soy manager, escribí\n");
        }
    }

    int mensaje;
    int read_response;
    while(true) {
        read_response = read(client_socket, &mensaje, sizeof(mensaje));
        if (read_response == -1) {
            perror("Error reading");
            exit(EXIT_FAILURE);
        } else if (read_response == 0) {
            printf("%i | Conexión finalizada.\n", client_socket);
            break;
        }

        printf("%i | Recibí el mensaje %i\n", client_socket, mensaje);

        safe(write(pipe_master[WRITE], &mensaje, sizeof(mensaje)));
        kill(getppid(), SIGINT);
        printf("%i | Listo!\n", client_socket);
    }

    close(client_socket);
    kill(reading_child_pid, SIGKILL);
    exit(EXIT_SUCCESS);
};

int main() {
    signal(SIGCHLD, connection_closed);
    signal(SIGINT, message_received);

    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un server_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_wpp"
    };
    safe_bind(server_socket, &server_addr, sizeof(server_addr));    
    
    for (int i = 0; i < MAX_CONNECTIONS; i++) safe(pipe(pipes[i]));
    safe(pipe(pipe_master));

    safe_listen2(server_socket, &server_addr, 0);
    int client_addr;
    socklen_t len = sizeof(client_addr);
    while (true) {
        int client_socket = accept(server_socket, (sockaddr_t) &client_addr, &len);
        if (connections_len == MAX_CONNECTIONS) {
            printf("Conexión rechazada debido a que se ha alcanzado el límite.\n");
            close(client_socket);
            continue;
        }
        if (client_socket == -1) continue;

        printf("escribiendo %i\n", client_socket);
        if (write(client_socket, &client_socket, sizeof(client_socket)) == -1) {
            perror("Error random");
            continue;
        };
        connections_len++;
        printf("%i | Conexión aceptada!\n", client_socket);
        // printf("Status | last_connection: %i | conexiones: %i\n", last_connection, connections_len);

        int index = -1;
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (connections[i] == 0) {
                index = i;
                break;
            }
        }
        int pid = fork();
        if (pid == 0) {
            manage_client(client_socket, index);
        } else {
            connections[index] = pid;
        }
    }
}